#ifndef BACNET_REMOTEOBJECTSTODEVICEMAPPER_H
#define BACNET_REMOTEOBJECTSTODEVICEMAPPER_H

#include "bacnetcommon.h"

namespace Bacnet {

class RemoteObjectsToDeviceMapper
{
public:
    RemoteObjectsToDeviceMapper(int maximumSize);
    quint32 findEntry(quint32 objectId, bool *found);
    void addOrUpdateRoutingEntry(quint32 objectIdNum, quint32 deviceObjectIdNum, bool forceAdd = false);

private:
    static const int TableSizeWarningLimit = 10;
    QHash<quint32, quint32> _mapperTable;
    int _tableMaximumSize;
};

} // namespace Bacnet

#endif // BACNET_REMOTEOBJECTSTODEVICEMAPPER_H
