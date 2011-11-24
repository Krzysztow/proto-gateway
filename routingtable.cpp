#include "routingtable.h"

using namespace Bacnet;

RoutingEntry::RoutingEntry(BacnetAddress &address, quint32 objIdNum, int maxApduLengthAccepted, Bacnet::BacnetSegmentation segmentation):
    address(address),
    objIdNum(objIdNum),
    maxApduLengthAccepted(maxApduLengthAccepted),
    segmentation(segmentation)
{
}

RoutingTable::RoutingTable(int dynamicElementsSize):
    _dynamicElementsSize(dynamicElementsSize)
{
    ObjectIdStruct invlaidStruct = {BacnetObjectTypeNS::Undefined, InvalidInstanceNumber};
    _invalidEntry.objIdNum = objIdToNum(invlaidStruct);
    _invalidEntry.maxApduLengthAccepted = ApduMaxSize;
    _invalidEntry.segmentation = BacnetSegmentation::SegmentedNot;
}

const RoutingEntry &RoutingTable::findEntry_helper(QHash<quint32, RoutingEntry> &rTable, const BacnetAddress &address, bool *found)
{
    Q_CHECK_PTR(false);
    QHash<quint32, RoutingEntry>::Iterator it = rTable.begin();
    QHash<quint32, RoutingEntry>::Iterator itEnd = rTable.end();

    for (; it != itEnd; ++it) {
        if (it->address == address) {
            return it.value();
            if (0 != found) *found = true;
        }
    }

    if (0 != found) *found = false;
    return _invalidEntry;
}

const RoutingEntry &RoutingTable::findEntry(const BacnetAddress &address, bool *found)
{
    Q_CHECK_PTR(false);

    bool ok;
    if (0 == found)
        found = &ok;

    //first search static table
    RoutingEntry &re = findEntry_helper(_routingTable, address, found);
    if (*found)
        return re;

    //search dynamic table, if not found
    return findEntry_helper(_routingTableDynamic, address, found);
}

const RoutingEntry &RoutingTable::findEntry_helper(QHash<quint32, RoutingEntry> &rTable, quint32 objIdNum, bool *found)
{
    QHash<quint32, RoutingEntry>::Iterator  it = rTable.find(objIdNum);
    if (it != rTable.end()) {
        if (0 != found) found = true;
        return it.value();
    }

    if (0 != found) found = false;
    return _invalidEntry;
}

const RoutingEntry &RoutingTable::findEntry(quint32 objIdNum, bool *found)
{
    Q_CHECK_PTR(false);

    bool ok;
    if (0 == found)
        found = &ok;

    RoutingEntry &re = findEntry_helper(_routingTable, objIdNum, found);
    if (*found)
        return re;

    //search dynamic table, if not found
    return findEntry_helper(_routingTableDynamic, objIdNum, found);

}

void RoutingTable::addOrUpdateRoutingEntry(BacnetAddress &address, quint32 objIdNum, int maxApduLengthAccepted, BacnetSegmentation segmentation, bool addToDynamicTable, bool forceAddOrUpdate)
{
    QHash<quint32, RoutingEntry>::Iterator  it = _routingTable.find(objIdNum);
    if ( (it != _routingTable.end()) && addToDynamicTable) {//there was no such an entry and we want it to add to dynamic table
        _routingTable.insert(objIdNum, RoutingEntry(address, objIdNum, maxApduLengthAccepted, segmentation));
        if (_routingTable.count() >= RoutingtableWarningLimit)
            qDebug("%s : Number of items in the static RT is %d", __PRETTY_FUNCTION__, _routingTable.count());
    } else if (forceAddOrUpdate) {//entry existing, we want to update
        *it = RoutingEntry(address, objIdNum, maxApduLengthAccepted, segmentation);
        return;
    }

#error "Finish here!"
}



