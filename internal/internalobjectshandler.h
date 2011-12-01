#ifndef ASYNCHSETTER_H
#define ASYNCHSETTER_H

#include "propertyowner.h"
#include "bacnetcommon.h"
#include "bacnetaddress.h"
#include "bacnetinternaladdresshelper.h"
#include "externalobjectshandler.h"

class BacnetService;
class Property;
class InternalRequestHandler;


namespace Bacnet {
    class BacnetDeviceObject;
    class BacnetObject;
    class BacnetTSM2;
    class SubscribeCOVServiceData;
    class CovSubscription;
    class PropertyValue;
    class BacnetApplicationLayerHandler;
    typedef QSharedPointer<PropertyValue> PropertyValueShared;

class InternalObjectsHandler
{
public:
    InternalObjectsHandler(BacnetApplicationLayerHandler *appLayer);

public://interface for BacnetObject-Internal interaction
    void propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
    void addAsynchronousHandler(QList<int> asynchIds, InternalRequestHandler *handler);

    void propertyValueChanged(BacnetObject *object, BacnetDeviceObject *device, CovSubscription &subscription, QList<PropertyValueShared> &propertiesValues);

public:
    bool addDevice(BacnetAddress &address, BacnetDeviceObject *device);
    QMap<quint32, BacnetDeviceObject*> &virtualDevices();

    //! \todo If performance here is bad, just return reference to QMap, as is stored.
    QList<BacnetDeviceObject*> devices();

public:
    QMap<InternalAddress, BacnetDeviceObject*> _devices;
    QHash<int, InternalRequestHandler*> _asynchRequests;
    BacnetApplicationLayerHandler *_appLayer;

//    /****************************
//          COV handling part
//    ****************************/
//public:
////    static const int MAX_TOTAL_COV_SUBSCRIPTIONS = 64;
////    void subscribeCOV(BacnetDeviceObject *device, BacnetAddress &requester, SubscribeCOVServiceData &covData, Error *error);

//private:

//    typedef QList<CovSubscription> TCovObjectSubscriptionList;
//    typedef QHash<BacnetObject*, TCovObjectSubscriptionList> TCovSubscriptionsHash;
//    typedef QHash<BacnetDeviceObject*, TCovSubscriptionsHash> TCovDevicesSubscriptions;
//    TCovDevicesSubscriptions _covSubscriptions;
//    int _totalCOVsubscriptionsNum;
};

}

#endif // ASYNCHSETTER_H
