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
    typedef QSharedPointer<PropertyValue> PropertyValueShared;
}

class InternalObjectsHandler
{
public:
    InternalObjectsHandler(Bacnet::BacnetTSM2 *tsm);

public://interface for BacnetObject-Internal interaction
    void propertyIoFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device);
    void addAsynchronousHandler(QList<int> asynchIds, InternalRequestHandler *handler);

    void propertyValueChanged(Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device, Bacnet::CovSubscription &subscription, QList<Bacnet::PropertyValueShared> &propertiesValues);

public:
    bool addDevice(InternalAddress address, Bacnet::BacnetDeviceObject *device);
    QMap<quint32, Bacnet::BacnetDeviceObject*> &virtualDevices();

    //! \todo If performance here is bad, just return reference to QMap, as is stored.
    QList<Bacnet::BacnetDeviceObject*> devices();

public:
    QMap<InternalAddress, Bacnet::BacnetDeviceObject*> _devices;
    QHash<int, InternalRequestHandler*> _asynchRequests;
    Bacnet::BacnetTSM2 *_tsm;

//    /****************************
//          COV handling part
//    ****************************/
//public:
////    static const int MAX_TOTAL_COV_SUBSCRIPTIONS = 64;
////    void subscribeCOV(BacnetDeviceObject *device, BacnetAddress &requester, Bacnet::SubscribeCOVServiceData &covData, Bacnet::Error *error);

//private:

//    typedef QList<CovSubscription> TCovObjectSubscriptionList;
//    typedef QHash<BacnetObject*, TCovObjectSubscriptionList> TCovSubscriptionsHash;
//    typedef QHash<BacnetDeviceObject*, TCovSubscriptionsHash> TCovDevicesSubscriptions;
//    TCovDevicesSubscriptions _covSubscriptions;
//    int _totalCOVsubscriptionsNum;
};


#endif // ASYNCHSETTER_H
