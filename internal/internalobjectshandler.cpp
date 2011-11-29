#include "internalobjectshandler.h"

#include "property.h"
#include "bacnetpci.h"
#include "bacnetservice.h"
#include "servicefactory.h"
#include "bacnetcommon.h"

#include <QDebug>

#include "bacnetreadpropertyack.h"
#include "helpercoder.h"
#include "bacnetdeviceobject.h"
#include "bacnetservice.h"

#include "helpercoder.h"
#include "analoginputobject.h"

#include "subscribecovservicedata.h"
#include "covnotificationrequestdata.h"
#include "covconfnotificationservicehandler.h"
#include "bacnetinternaladdresshelper.h"
#include "internalrequesthandler.h"
#include "bacnetapplicationlayer.h"

//void AsynchSetter::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
void InternalObjectsHandler::propertyIoFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device)
{
    Q_ASSERT(_asynchRequests.contains(asynchId));

    Q_CHECK_PTR(device);
    Q_CHECK_PTR(object);

    InternalRequestHandler *serviceActHndlr = _asynchRequests.take(asynchId);
    if (!serviceActHndlr->asynchActionFinished(asynchId, result, object, device)) {
        return;
    }

    bool deleteHandler(true);
    serviceActHndlr->finalize(&deleteHandler);
    if (deleteHandler) {
        delete serviceActHndlr;
    }
}

void InternalObjectsHandler::propertyValueChanged(Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device, Bacnet::CovSubscription &subscription, QList<Bacnet::PropertyValueShared> &propertiesValues)
{
    Q_CHECK_PTR(device);
    Q_CHECK_PTR(object);

    Q_ASSERT(object->objectIdNum() == subscription._monitoredPropertyRef.objId().objectIdNum());

    Bacnet::CovNotificationRequestData *covData = new Bacnet::CovNotificationRequestData(subscription._recipientProcess.processId(), device->objectId(), object->objectId(),
                                                                                         subscription._timeLeft);

    //add properties list
    for (int i = 0; i < propertiesValues.count(); ++i) {
        covData->_listOfValues.append(propertiesValues[i]);
    }

    BacnetAddress devAddress = device->address();

    //! \todo Unify send interface!
    //send it
    if (subscription.isIssueConfirmedNotifications()) {
        Bacnet::CovConfNotificationServiceHandler *hndlr = new Bacnet::CovConfNotificationServiceHandler(covData);//takes ownership
        _appLayer->send(subscription.recipientAddress()->address(), devAddress, hndlr);
//        if (subscription.recipientHasAddress()) {
//            _appLayer->send(subscription.recipientAddress()->address(), devAddress, BacnetServicesNS::ConfirmedCOVNotification, hndlr);
//        } else {
//            Q_ASSERT(0 != subscription.recipientObjId());
//            _appLayer->send(subscription.recipientObjId()->_value, device->address(), BacnetServicesNS::ConfirmedCOVNotification, hndlr);
//        }
#warning "Get rid of _value dependency (ObjIdStruct)"
    } else {
        if (subscription.recipientHasAddress()) {
            _appLayer->sendUnconfirmed(subscription.recipientAddress()->address(), devAddress, *covData, BacnetServicesNS::UnconfirmedCOVNotification);
        } else {
            Q_ASSERT(0 != subscription.recipientObjId());
            _appLayer->sendUnconfirmed(subscription.recipientObjId()->_value, devAddress, *covData, BacnetServicesNS::UnconfirmedCOVNotification);
        }

        //was sent, now has to be destroyed!
        delete covData;
    }

////    typedef QList<Bacnet::SubscribeCOVServiceData> TCovObjectSubscriptionList;
////    typedef QHash<BacnetObject*, TCovObjectSubscriptionList> TCovSubscriptionsHash;
////    typedef QHash<BacnetDeviceObject*, TCovSubscriptionsHash> TCovDevicesSubscriptions;

//    TCovDevicesSubscriptions::Iterator devIt = _covSubscriptions.find(device);
//    if (devIt == _covSubscriptions.end())
//        return;//nothing subscribed

//    TCovSubscriptionsHash::Iterator subObjIt = (*devIt).find(object);
//    if (subObjIt == (*devIt).end())
//        return;//no object subscription

//    TCovObjectSubscriptionList::Iterator subscriptionsIt = (*subObjIt).begin();
//    CovNotificationRequestData *covData(0);
//    //inform each subscriber that value have changed!
//    for (; subscriptionsIt != (*subObjIt).end(); ++subscriptionsIt) {
//        //! \todo We could have optimization here - get the value only once, instead of for each subscribed device. Then SharedData should be used as well.
//        QList<Bacnet::PropertyValue*> dataList = object->readCovValuesList();
//        if (dataList.isEmpty()) {
//            qDebug("InternalObjectsHandler::propertyValueChanged() : data changed but we got zero pointer dev (0x%x), obj (0x%x), propId (0x%x)",
//                   device->objectIdNum(), object->objectIdNum(), propId);
//            Q_ASSERT(false);
//            return;
//        }

//        if (0 == covData)
//            covData = new CovNotificationRequestData((*subscriptionsIt)._subscriberProcId, device->objectId(),
//                                                     (*subscriptionsIt)._monitoredObjectId, (*subscriptionsIt)._timeLeft);

//        foreach (Bacnet::PropertyValue *data, dataList) {
//            covData->_listOfValues.append(data);
//        }

//        if ((*subscriptionsIt)._issueConfNotification) {
//            //send
//            CovConfNotificationServiceHandler *hndlr = new CovConfNotificationServiceHandler(covData);//takes ownership
//            covData = 0;//so that next time, new one is created.
//            _appLayer->send((*subscriptionsIt)._subscriberAddress, , BacnetServices::ConfirmedCOVNotification, hndlr);
//        } else {
//            _appLayer->sendUnconfirmed((*subscriptionsIt)._subscriberAddress, , *covData, BacnetServices::UnconfirmedCOVNotification);
//        }
//    }
}

void InternalObjectsHandler::addAsynchronousHandler(QList<int> asynchIds, InternalRequestHandler *handler)
{
    foreach (int asynchId, asynchIds) {
        Q_ASSERT(!_asynchRequests.contains(asynchId));
        _asynchRequests.insert(asynchId, handler);
    }
}

bool InternalObjectsHandler::addDevice(BacnetAddress &address, Bacnet::BacnetDeviceObject *device)
{
    InternalAddress intAddress = BacnetInternalAddressHelper::internalAddress(address);
    Q_ASSERT(!_devices.contains(intAddress));
    Q_CHECK_PTR(device);
    if ( (BacnetInternalAddressHelper::InvalidInternalAddress == intAddress) || _devices.contains(intAddress))
        return false;

    _devices.insert(intAddress, device);
    device->setHandler(this);
    return true;
}

QMap<quint32, Bacnet::BacnetDeviceObject*> &InternalObjectsHandler::virtualDevices()
{
    return _devices;
}

QList<Bacnet::BacnetDeviceObject*> InternalObjectsHandler::devices()
{
    return _devices.values();
}

InternalObjectsHandler::InternalObjectsHandler(Bacnet::BacnetApplicationLayerHandler *appLayer):
    _appLayer(appLayer)
{
    Q_CHECK_PTR(_appLayer);
}

//void InternalObjectsHandler::subscribeCOV(BacnetDeviceObject *device, BacnetAddress &requester, Bacnet::SubscribeCOVServiceData &covData, Bacnet::Error *error)
//{
//    Q_CHECK_PTR(error);
//    Q_CHECK_PTR(device);

//    BacnetObject *object = device->bacnetObject(objIdToNum(covData._monitoredObjectId));
//    Q_CHECK_PTR(object);//this is not a software error, but a requester mistake. Leave it just for now.
//    if (0 == object) {
//        error->setError(BacnetErrorNS::ClassObject, BacnetErrorNS::CodeUnknownObject);
//        return;
//    }

//    //we have found the object.
//    TCovDevicesSubscriptions::Iterator devSubscirptionIt;;
//    TCovSubscriptionsHash::Iterator objSubscriptionHashIt;

//    //initialize those iterators that are reachable.
//    devSubscirptionIt = _covSubscriptions.find(device);
//    if (devSubscirptionIt != _covSubscriptions.end()) {
//        objSubscriptionHashIt = (*devSubscirptionIt).find(object);
//    }

//    if (!covData.isLifetimePresent() && !covData.isConfirmedNotificationPresent()) {//unsubscription request
//        if ( (devSubscirptionIt == _covSubscriptions.end()) ||
//             (objSubscriptionHashIt == (*devSubscirptionIt).end()) ||
//             ((*objSubscriptionHashIt).isEmpty()) )//we don't have to do anything - no such subscription.
//            return;

//        //there are some subscriptions for this object, check if we have a match
//        TCovObjectSubscriptionList::Iterator objSubscriptionIt = (*objSubscriptionHashIt).begin();
//        for (; objSubscriptionIt != (*objSubscriptionHashIt).end(); ++objSubscriptionIt) {
//            if ((*objSubscriptionIt).compareSubscriptions(covData)) //have we found appropriate entry?
//                break;
//        }
//        if (objSubscriptionIt != (*objSubscriptionHashIt).end()) {//we have found matching entry.
//            (*objSubscriptionHashIt).erase(objSubscriptionIt);
//            //if there are no more subcriptions associated with the device, clean some hashes and lists.
//            if ((*objSubscriptionHashIt).isEmpty()) {
//                devSubscirptionIt->remove(object);
//                if(devSubscirptionIt->isEmpty()) {
//                    _covSubscriptions.erase(devSubscirptionIt);
//                    --_totalCOVsubscriptionsNum;
//                }
//            }
//        }
//        return;//all done
//    }
//    if (!covData.isLifetimePresent() || !covData.isConfirmedNotificationPresent()) {
//        error->setError(BacnetErrorNS::ClassServices, BacnetErrorNS::CodeInconsistentParameters);
//        return;
//    }

//    //that was a subscription request
//    if (devSubscirptionIt == _covSubscriptions.end()) {//no device subscriptions has been done yet
//        devSubscirptionIt = _covSubscriptions.insert(device, TCovSubscriptionsHash());
//        objSubscriptionHashIt = devSubscirptionIt->end();
//    }

//    if (objSubscriptionHashIt == (*devSubscirptionIt).end()) {
//        objSubscriptionHashIt = (*devSubscirptionIt).insert(object, TCovObjectSubscriptionList());
//    }

//    TCovObjectSubscriptionList::Iterator objSubscriptionListIt = (*objSubscriptionHashIt).begin();
//    for (; objSubscriptionListIt != (*objSubscriptionHashIt).end(); ++objSubscriptionListIt) {
//        if ((*objSubscriptionListIt).compareSubscriptions(covData))
//            break;
//    }
//    if (objSubscriptionListIt != (*objSubscriptionHashIt).end()) {//was not found, we have to add it.
//        if (MAX_TOTAL_COV_SUBSCRIPTIONS == _totalCOVsubscriptionsNum) {//haven't we exceeded a maximum number of subscriptions possible?
//            error->setError(BacnetErrorNS::ClassServices, BacnetErrorNS::CodeCovSubscriptionFailed);
//            return;
//        }
//        (*objSubscriptionHashIt).append(CovSubscription(covData, requester));
//    } else {//the subscription is already there. That means we have to update it.
//        objSubscriptionListIt->_issueConfNotification = covData._issueConfNotification;
//        objSubscriptionListIt->_timeLeft = covData._lifetime;
//    }
//}
