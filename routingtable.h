#ifndef BACNET_ROUTINGTABLE_H
#define BACNET_ROUTINGTABLE_H

#include "bacnetcommon.h"
#include "bacnetaddress.h"

namespace Bacnet {

class mappingEntry {
public:
    mappingEntry();
    mappingEntry(BacnetAddress &address, quint32 devObjIdNum,int maxApduLengthAccepted, BacnetSegmentation segmentation);

public:
    BacnetAddress address;
    quint32 devObjIdNum;
    int maxApduLengthAccepted;
    BacnetSegmentation segmentation;
};

class RoutingTable
{
public:
    RoutingTable(int dynamicElementsSize);

    const mappingEntry &findEntry(const BacnetAddress &address, bool *found);
    const mappingEntry &findEntry(quint32 objIdNum, bool *found);

    //! Adds entry to one of the internal lists. Returns true, if the element was already in the table.
    bool addOrUpdatemappingEntry(BacnetAddress &address, quint32 devObjIdNum, int maxApduLengthAccepted, BacnetSegmentation segmentation, bool addToDynamicTable = true, bool forceAdd = true);

private:
    const mappingEntry &findEntry_helper(QHash<quint32, mappingEntry> &rTable, const BacnetAddress &address, bool *found);
    const mappingEntry &findEntry_helper(QHash<quint32, mappingEntry> &rTable, ObjIdNum devObjjIdNum, bool *found);

private:
    static const int RoutingtableWarningLimit = 10;
    QHash<quint32, mappingEntry> _routingTable;
    QHash<quint32, mappingEntry> _routingTableDynamic;

    mappingEntry _invalidEntry;
    int _dynamicElementsSize;
};

} // namespace Bacnet

#endif // BACNET_ROUTINGTABLE_H
