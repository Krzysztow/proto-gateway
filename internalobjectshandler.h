#ifndef ASYNCHSETTER_H
#define ASYNCHSETTER_H

#include "propertyowner.h"
#include "bacnetcommon.h"
#include "bacnetaddress.h"
#include "bacnetinternaladdresshelper.h"
#include "externalobjectshandler.h"

class BacnetDeviceObject;
class BacnetObject;
class BacnetService;
class Property;
class InternalRequestHandler;


namespace Bacnet {
    class BacnetTSM2;
    class SubscribeCOVServiceData;
}

class InternalObjectsHandler
{
public:
    InternalObjectsHandler(Bacnet::BacnetTSM2 *tsm);

public://interface for BacnetObject-Internal interaction
    void propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
    void propertyValueChanged(BacnetObject *object, BacnetDeviceObject *device, BacnetProperty::Identifier propId);
    void addAsynchronousHandler(QList<int> asynchIds, InternalRequestHandler *handler);

public:
    bool addDevice(InternalAddress address, BacnetDeviceObject *device);
    QMap<quint32, BacnetDeviceObject*> &virtualDevices();

    //! \todo If performance here is bad, just return reference to QMap, as is stored.
    QList<BacnetDeviceObject*> devices();

public:
    QMap<InternalAddress, BacnetDeviceObject*> _devices;
    QHash<int, InternalRequestHandler*> _asynchRequests;
    Bacnet::BacnetTSM2 *_tsm;

    /****************************
          COV handling part
    ****************************/
public:
//    static const int MAX_TOTAL_COV_SUBSCRIPTIONS = 64;
//    void subscribeCOV(BacnetDeviceObject *device, BacnetAddress &requester, Bacnet::SubscribeCOVServiceData &covData, Bacnet::Error *error);

private:

    typedef QList<CovSubscription> TCovObjectSubscriptionList;
    typedef QHash<BacnetObject*, TCovObjectSubscriptionList> TCovSubscriptionsHash;
    typedef QHash<BacnetDeviceObject*, TCovSubscriptionsHash> TCovDevicesSubscriptions;
    TCovDevicesSubscriptions _covSubscriptions;
    int _totalCOVsubscriptionsNum;
};


#endif // ASYNCHSETTER_H
