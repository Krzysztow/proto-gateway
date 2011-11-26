#include "remoteobjectstodevicemapper.h"

using namespace Bacnet;

RemoteObjectsToDeviceMapper::RemoteObjectsToDeviceMapper(int maximumSize):
    _tableMaximumSize(maximumSize)
{
    if (0 == _tableMaximumSize)
        qDebug("%s : Object to device mapper size is not limited", __PRETTY_FUNCTION__);
}

quint32 RemoteObjectsToDeviceMapper::findEntry(quint32 objectId, bool *found)
{
    quint32 ret = _mapperTable.value(objectId);
    if (0 == ret) { //element not foud
        if (0 != found) *found = false;
        return invalidObjIdNum();
    } else { //found
        if (0 != found) *found = true;
        return ret;
    }
}

bool RemoteObjectsToDeviceMapper::addOrUpdateRoutingEntry(quint32 objectIdNum, quint32 deviceObjectIdNum, bool forceAdd)
{
    Q_ASSERT(BacnetObjectTypeNS::Device == numToObjId(deviceObjectIdNum).objectType);
    QHash<quint32, quint32>::Iterator it = _mapperTable.find(objectIdNum);
    if (_mapperTable.end() != it) { //we found it, update
        it.value() = deviceObjectIdNum;
        return true;
    } else { //was not found, we need to add
        int count = _mapperTable.count();
        if (_tableMaximumSize >= count) {
            _mapperTable.insert(objectIdNum, deviceObjectIdNum);
        } else if (forceAdd) { //there is no enough place, but caller told us to force it - replace with some random victim.
            int victimNum = qrand() % count;
            _mapperTable.erase(_mapperTable.begin() + victimNum);
            Q_ASSERT(!_mapperTable.contains(objectIdNum));
            _mapperTable.insert(objectIdNum, deviceObjectIdNum);
        } else
            qDebug("%s : Therer was no enough place to add the entry", __PRETTY_FUNCTION__);

        if (count > TableSizeWarningLimit)
            qDebug("%s : mapper table size is %d", __PRETTY_FUNCTION__, count);
    }

    return false;
}
