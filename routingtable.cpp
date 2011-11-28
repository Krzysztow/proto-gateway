#include "routingtable.h"

using namespace Bacnet;

mappingEntry::mappingEntry():
    //address gets default, uninitialized constructor
    devObjIdNum(Bacnet::invalidObjIdNum()),
    maxApduLengthAccepted(ApduMaxSize),
    segmentation(Bacnet::SegmentedNOT)
{
}

mappingEntry::mappingEntry(BacnetAddress &address, ObjIdNum devObjjIdNum, int maxApduLengthAccepted, Bacnet::BacnetSegmentation segmentation):
    address(address),
    devObjIdNum(devObjjIdNum),
    maxApduLengthAccepted(maxApduLengthAccepted),
    segmentation(segmentation)
{
}

RoutingTable::RoutingTable(int dynamicElementsSize):
    _dynamicElementsSize(dynamicElementsSize)
{
}

const mappingEntry &RoutingTable::findEntry_helper(QHash<quint32, mappingEntry> &rTable, const BacnetAddress &address, bool *found)
{
    Q_CHECK_PTR(false);
    QHash<quint32, mappingEntry>::Iterator it = rTable.begin();
    QHash<quint32, mappingEntry>::Iterator itEnd = rTable.end();

    for (; it != itEnd; ++it) {
        if (it->address == address) {
            return it.value();
            if (0 != found) *found = true;
        }
    }

    if (0 != found) *found = false;
    return _invalidEntry;
}

const mappingEntry &RoutingTable::findEntry(const BacnetAddress &address, bool *found)
{
    Q_CHECK_PTR(false);

    bool ok;
    if (0 == found)
        found = &ok;

    //first search static table
    const mappingEntry &re = findEntry_helper(_routingTable, address, found);
    if (*found)
        return re;

    //search dynamic table, if not found
    return findEntry_helper(_routingTableDynamic, address, found);
}

const mappingEntry &RoutingTable::findEntry_helper(QHash<quint32, mappingEntry> &rTable, quint32 objIdNum, bool *found)
{
    QHash<quint32, mappingEntry>::Iterator  it = rTable.find(objIdNum);
    if (it != rTable.end()) {
        if (0 != found) *found = true;
        return it.value();
    }

    if (0 != found) *found = false;
    return _invalidEntry;
}

const mappingEntry &RoutingTable::findEntry(quint32 objIdNum, bool *found)
{
    bool ok;
    if (0 == found)
        found = &ok;

    const mappingEntry &re = findEntry_helper(_routingTable, objIdNum, found);
    if (*found)
        return re;

    //search dynamic table, if not found
    return findEntry_helper(_routingTableDynamic, objIdNum, found);

}

bool RoutingTable::addOrUpdatemappingEntry(BacnetAddress &address, quint32 devObjIdNum, int maxApduLengthAccepted, BacnetSegmentation segmentation, bool toDynamicTable, bool forceAddOrUpdate)
{
    QHash<quint32, mappingEntry>::Iterator  it = _routingTable.find(devObjIdNum);
    if (_routingTable.end() != it) {//there was such an entry, so don't care about toDynamicTable flag and...
        if (forceAddOrUpdate) {//...and we want it to be updated.
            *it = mappingEntry(address, devObjIdNum, maxApduLengthAccepted, segmentation);
        }
        return true;
    } else if (!toDynamicTable) {//entry was not existing - we check, if the user wanted to insert it to the static tabe - !toDynamicTable
        _routingTable.insert(devObjIdNum, mappingEntry(address, devObjIdNum, maxApduLengthAccepted, segmentation));
        if (_routingTable.count() >= RoutingtableWarningLimit)
            qDebug("%s : Number of items in the static RT is %d", __PRETTY_FUNCTION__, _routingTable.count());
        return false;
    }

    //being here means the entry wasn't destined to the dynamic entry
    Q_ASSERT(toDynamicTable);
    it = _routingTableDynamic.find(devObjIdNum);
    if (_routingTableDynamic.end() != it) {//the entry was there
        if (forceAddOrUpdate) {
            *it = mappingEntry(address, devObjIdNum, maxApduLengthAccepted, segmentation);
        }
        return true;
    } else {
        if (_routingTableDynamic.count() <= _dynamicElementsSize) {
            _routingTableDynamic.insert(devObjIdNum, mappingEntry(address, devObjIdNum, maxApduLengthAccepted, segmentation));
        } else if (forceAddOrUpdate) { //we have to insert this entry - choose some victim and replace it
            int victimNum = qrand()%_routingTableDynamic.count();
            it = _routingTableDynamic.begin();
            it += victimNum;
            Q_ASSERT(_routingTableDynamic.contains(it.key()));
            _routingTableDynamic.erase(it);
            Q_ASSERT(!_routingTableDynamic.contains(it.key()));
            _routingTableDynamic.insert(devObjIdNum, mappingEntry(address, devObjIdNum, maxApduLengthAccepted, segmentation));
        } else
            qDebug("%s : Didn't have more space to insert routing entry and was not forced to replace with some other one", __PRETTY_FUNCTION__);
    }

    return false;
}



