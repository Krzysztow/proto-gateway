#ifndef BACNET_ROUTINGTABLE_H
#define BACNET_ROUTINGTABLE_H

#include "bacnetcommon.h"
#include "bacnetaddress.h"

namespace Bacnet {

class RoutingEntry {
public:
    RoutingEntry();
    RoutingEntry(BacnetAddress &address, quint32 objIdNum,int maxApduLengthAccepted, BacnetSegmentation segmentation);

public:
    BacnetAddress address;
    quint32 objIdNum;
    int maxApduLengthAccepted;
    BacnetSegmentation segmentation;
};
class RoutingTable
{
public:
    RoutingTable(int dynamicElementsSize);

    const RoutingEntry &findEntry(const BacnetAddress &address, bool *found);
    const RoutingEntry &findEntry(quint32 objIdNum, bool *found);

    void addOrUpdateRoutingEntry(BacnetAddress &address, quint32 objIdNum, int maxApduLengthAccepted, BacnetSegmentation segmentation, bool addToDynamicTable = true, bool forceAdd = true);

private:
    const RoutingEntry &findEntry_helper(QHash<quint32, RoutingEntry> &rTable, const BacnetAddress &address, bool *found);
    const RoutingEntry &findEntry_helper(QHash<quint32, RoutingEntry> &rTable, quint32 objIdNum, bool *found);

private:
    static const int RoutingtableWarningLimit = 10;
    QHash<quint32, RoutingEntry> _routingTable;
    QHash<quint32, RoutingEntry> _routingTableDynamic;

    RoutingEntry _invalidEntry;
    int _dynamicElementsSize;
};

} // namespace Bacnet

#endif // BACNET_ROUTINGTABLE_H
