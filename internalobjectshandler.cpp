#include "internalobjectshandler.h"

#include "property.h"
#include "bacnetpci.h"
#include "bacnetservice.h"
#include "servicefactory.h"
#include "bacnetcommon.h"

#include <QDebug>

#include "bacnetreadpropertyack.h"
#include "helpercoder.h"
#include "bacnetwritepropertyservice.h"
#include "bacnetdeviceobject.h"
#include "bacnetservice.h"
#include "bacnettsm2.h"

#include "helpercoder.h"
#include "analoginputobject.h"

#include "subscribecovservicedata.h"
#include "covnotificationrequestdata.h"
#include "covconfnotificationservicehandler.h"
#include "bacnetinternaladdresshelper.h"

//void AsynchSetter::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
void InternalObjectsHandler::propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
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

void InternalObjectsHandler::propertyValueChanged(BacnetObject *object, BacnetDeviceObject *device, Bacnet::CovSubscription &subscription, QList<Bacnet::PropertyValueShared> &propertiesValues)
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

    BacnetAddress devAddress = BacnetInternalAddressHelper::toBacnetAddress(device->address());

    //! \todo Unify send interface!
    //send it
    if (subscription.isIssueConfirmedNotifications()) {
        Bacnet::CovConfNotificationServiceHandler *hndlr = new Bacnet::CovConfNotificationServiceHandler(covData);//takes ownership
        if (subscription.recipientHasAddress()) {
            _tsm->send(subscription.recipientAddress()->address(), devAddress, BacnetServices::ConfirmedCOVNotification, hndlr);
        } else {
            Q_ASSERT(0 != subscription.recipientObjId());
            _tsm->send(subscription.recipientObjId()->_value, device->address(), BacnetServices::ConfirmedCOVNotification, hndlr);
        }
#warning "Get rid of _value dependency (ObjIdStruct)"
    } else {
        if (subscription.recipientHasAddress()) {
            _tsm->sendUnconfirmed(subscription.recipientAddress()->address(), devAddress, *covData, BacnetServices::UnconfirmedCOVNotification);
        } else {
            Q_ASSERT(0 != subscription.recipientObjId());
            _tsm->sendUnconfirmed(subscription.recipientObjId()->_value, devAddress, *covData, BacnetServices::UnconfirmedCOVNotification);
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
//            _tsm->send((*subscriptionsIt)._subscriberAddress, , BacnetServices::ConfirmedCOVNotification, hndlr);
//        } else {
//            _tsm->sendUnconfirmed((*subscriptionsIt)._subscriberAddress, , *covData, BacnetServices::UnconfirmedCOVNotification);
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

bool InternalObjectsHandler::addDevice(InternalAddress address, BacnetDeviceObject *device)
{
    Q_ASSERT(!_devices.contains(address));
    if ( (BacnetInternalAddressHelper::InvalidInternalAddress == address) || _devices.contains(address))
        return false;

    _devices.insert(address, device);
    device->setHandler(this);
    return true;
}

QMap<quint32, BacnetDeviceObject*> &InternalObjectsHandler::virtualDevices()
{
    return _devices;
}

QList<BacnetDeviceObject*> InternalObjectsHandler::devices()
{
    return _devices.values();
}

InternalObjectsHandler::InternalObjectsHandler(Bacnet::BacnetTSM2 *tsm):
    _tsm(tsm)
{
    Q_CHECK_PTR(_tsm);
}

//void InternalObjectsHandler::subscribeCOV(BacnetDeviceObject *device, BacnetAddress &requester, Bacnet::SubscribeCOVServiceData &covData, Bacnet::Error *error)
//{
//    Q_CHECK_PTR(error);
//    Q_CHECK_PTR(device);

//    BacnetObject *object = device->bacnetObject(objIdToNum(covData._monitoredObjectId));
//    Q_CHECK_PTR(object);//this is not a software error, but a requester mistake. Leave it just for now.
//    if (0 == object) {
//        error->setError(BacnetError::ClassObject, BacnetError::CodeUnknownObject);
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
//        error->setError(BacnetError::ClassServices, BacnetError::CodeInconsistentParameters);
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
//            error->setError(BacnetError::ClassServices, BacnetError::CodeCovSubscriptionFailed);
//            return;
//        }
//        (*objSubscriptionHashIt).append(CovSubscription(covData, requester));
//    } else {//the subscription is already there. That means we have to update it.
//        objSubscriptionListIt->_issueConfNotification = covData._issueConfNotification;
//        objSubscriptionListIt->_timeLeft = covData._lifetime;
//    }
//}
