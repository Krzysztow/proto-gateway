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
        _tsm(tsm),
        _totalCOVsubscriptionsNum(0)
{
    Q_CHECK_PTR(_tsm);
}

void InternalObjectsHandler::subscribeCOV(BacnetDeviceObject *device, Bacnet::SubscribeCOVServiceData &covData, Bacnet::Error *error)
{
    Q_CHECK_PTR(error);
    Q_CHECK_PTR(device);

    BacnetObject *object = device->bacnetObject(objIdToNum(covData._monitoredObjectId));
    Q_CHECK_PTR(object);//this is not a software error, but a requester mistake. Leave it just for now.
    if (0 == object) {
        error->setError(BacnetError::ClassObject, BacnetError::CodeUnknownObject);
        return;
    }

    //we have found the object.
    TCovDevicesSubscriptions::Iterator devSubscirptionIt;;
    TCovSubscriptionsHash::Iterator objSubscriptionHashIt;

    //initialize those iterators that are reachable.
    devSubscirptionIt = _covSubscriptions.find(device);
    if (devSubscirptionIt != _covSubscriptions.end()) {
        objSubscriptionHashIt = (*devSubscirptionIt).find(object);
    }

    if (!covData.isLifetimePresent() && !covData.isConfirmedNotificationPresent()) {//unsubscription request
        if ( (devSubscirptionIt == _covSubscriptions.end()) ||
             (objSubscriptionHashIt == (*devSubscirptionIt).end()) ||
             ((*objSubscriptionHashIt).isEmpty()) )//we don't have to do anything - no such subscription.
            return;

        //there are some subscriptions for this object, check if we have a match
        TCovObjectSubscriptionList::Iterator objSubscriptionIt = (*objSubscriptionHashIt).begin();
        for (; objSubscriptionIt != (*objSubscriptionHashIt).end(); ++objSubscriptionIt) {
            if ((*objSubscriptionIt).compareSubscriptions(covData)) //have we found appropriate entry?
                break;
        }
        if (objSubscriptionIt != (*objSubscriptionHashIt).end()) {//we have found matching entry.
            (*objSubscriptionHashIt).erase(objSubscriptionIt);
            //if there are no more subcriptions associated with the device, clean some hashes and lists.
            if ((*objSubscriptionHashIt).isEmpty()) {
                devSubscirptionIt->remove(object);
                if(devSubscirptionIt->isEmpty()) {
                    _covSubscriptions.erase(devSubscirptionIt);
                    --_totalCOVsubscriptionsNum;
                }
            }
        }
        return;//all done
    }
    if (!covData.isLifetimePresent() || !covData.isConfirmedNotificationPresent()) {
        error->setError(BacnetError::ClassServices, BacnetError::CodeInconsistentParameters);
        return;
    }

    //that was a subscription request
    if (devSubscirptionIt == _covSubscriptions.end()) {//no device subscriptions has been done yet
        devSubscirptionIt = _covSubscriptions.insert(device, TCovSubscriptionsHash());
        objSubscriptionHashIt = devSubscirptionIt->end();
    }

    if (objSubscriptionHashIt == (*devSubscirptionIt).end()) {
        objSubscriptionHashIt = (*devSubscirptionIt).insert(object, TCovObjectSubscriptionList());
    }

    TCovObjectSubscriptionList::Iterator objSubscriptionListIt = (*objSubscriptionHashIt).begin();
    for (; objSubscriptionListIt != (*objSubscriptionHashIt).end(); ++objSubscriptionListIt) {
        if ((*objSubscriptionListIt).compareSubscriptions(covData))
            break;
    }
    if (objSubscriptionListIt != (*objSubscriptionHashIt).end()) {//was not found, we have to add it.
        if (MaxTotaCOVSubscriptions == _totalCOVsubscriptionsNum) {//haven't we exceeded a maximum number of subscriptions possible?
            error->setError(BacnetError::ClassServices, BacnetError::CodeCovSubscriptionFailed);
            return;
        }
        (*objSubscriptionHashIt).append(covData);
    } else {//the subscription is already there. That means we have to update it.
        objSubscriptionListIt->_issueConfNotification = covData._issueConfNotification;
        objSubscriptionListIt->_lifetime = covData._lifetime;
    }
}
