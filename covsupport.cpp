#include "covsupport.h"

#include "error.h"
#include "subscribecovservicedata.h"
#include "bacnetaddress.h"
#include "covincrementhandlers.h"
#include "propertyvalue.h"
#include "bacnetobject.h"
#include "bacnetdeviceobject.h"
#include "covsubscriptionstimehandler.h"

using namespace Bacnet;

CovSupport::CovSupport()
{
}

CovSupport::~CovSupport()
{
    qDeleteAll(_incrementHandlers);
    _incrementHandlers.clear();
}

void CovSupport::addOrUpdateCovSubscription(Bacnet::SubscribeCOVServiceData &covData, BacnetAddress &requester, Bacnet::Error *error)
{
    Q_CHECK_PTR(error);

    //is property supported?
    if (!covProperties().contains(covData._propReference->propIdentifier())) {
        qDebug("%s : Property is not cov supported", __PRETTY_FUNCTION__);
        error->setError(BacnetErrorNS::ClassObject, BacnetErrorNS::CodeNotCovProperty);
        return;
    }

    //first try to find such an existing subscription
    Bacnet::CovSubscription *subscription(0);
    foreach (Bacnet::CovSubscription subscr, _subscriptions) {
        //! \warning Comparison is only done by requester address, not requester deviceIdentifier. It could be dangerous, when we make subscriptions with configuration files
        // Then most probably we want to use device identifier, not address.
        if (subscr.compareParametrs(requester, covData._subscriberProcId,
                                    covData._monitoredObjectId, covData._propReference->propIdentifier(), covData._propReference->propArrayIndex())) {
            //such subscription already exists, we take reference to it and will update values later.
            subscription = &subscr;
            break;
        }
    }

    if (0 == subscription) {
        // \warning Let's see how QList handles this case - if temporarily creted CovSubscription is deleted - it will delete it's handler as well, so that copied and appended to list handler points to deallocated memory.
        qDebug("%s : Warning - we have double copy & destruction here, it could delete increment handler!", __PRETTY_FUNCTION__);
        _subscriptions.append(Bacnet::CovSubscription(covData, requester));
        subscription = &(_subscriptions.last());
    }

    Q_ASSERT(0 != subscription);
    if (subscription == 0) {
        error->setError(BacnetErrorNS::ClassResources, BacnetErrorNS::CodeOther);
        return;
    }

    subscription->update(covData._lifetime, covData.takeCovIncrement());

    //! \todo some optimization would be useful here, so that isntance doesn't subscribe itself to timeHandler, if is already subscribed.
    updateWithTimeHandlerHelper();
}

void CovSupport::rmCovSubscription(quint32 processId, BacnetAddress &requester, Bacnet::ObjectIdentifier &monitoredObjectId, Bacnet::PropertyReference &propReference, Bacnet::Error *error)
{
    Q_CHECK_PTR(error);

    QList<Bacnet::CovSubscription>::Iterator it = _subscriptions.begin();
    QList<Bacnet::CovSubscription>::Iterator end = _subscriptions.end();
    for (; it != end; ++it) {
        //! \warning Comparison is only done by requester address, not requester deviceIdentifier. It could be dangerous, when we make subscriptions with configuration files
        // Then most probably we want to use device identifier, not address.
        if (it->compareParametrs(requester, processId, monitoredObjectId, propReference.propIdentifier(), propReference.propArrayIndex()))
            break;
    }

    if (it != end) {
        //such subscription exists, indeed.
        qDebug("%s : subscription removed.", __PRETTY_FUNCTION__);
        _subscriptions.erase(it);

        //update subscription with timer handler.
        updateWithTimeHandlerHelper();
    }
}

void CovSupport::addCovIncrementHandler(BacnetPropertyNS::Identifier propId, CovRealIcnrementHandler *incrementHandler)
{
    QHash<BacnetPropertyNS::Identifier, CovRealIcnrementHandler*>::Iterator it = _incrementHandlers.find(propId);
    if (it != _incrementHandlers.end()) {
        //delete old property
        delete it.value();
        it.value() = 0;
    }

    if (incrementHandler != 0) {
        //if the new handler is not null, insert it
        if (it != _incrementHandlers.end())
            it.value() = incrementHandler;
        else
            _incrementHandlers.insert(propId, incrementHandler);
    } else {
        //remove hash key as well
        _incrementHandlers.erase(it);
    }
}

CovRealIcnrementHandler *CovSupport::covIncrementHandler(BacnetPropertyNS::Identifier propId)
{
    return _incrementHandlers[propId];
}

CovRealIcnrementHandler *CovSupport::takeCovIncrementHandler(BacnetPropertyNS::Identifier propId)
{
    return _incrementHandlers.take(propId);
}

void CovSupport::remvoeCovIncrementHandler(BacnetPropertyNS::Identifier propId)
{
    //this will delete increment handler and remove the item
    addCovIncrementHandler(propId, 0);
}

//bool CovSupport::valueChanged(BacnetPropertyNS::Identifier propId, Bacnet::BacnetDataInterface *value)
//{
//    CovRealIcnrementHandler *propertyCovHandler = _incrementHandlers[propId];
//    if (0 == propertyCovHandler)//if there is no increment handler - assume, value has changed at least by increment.
//        return true;

//    value->accept(propertyCovHandler);
//    if (!propertyCovHandler->isEqualWithinIncrement())//are we changed more than increment specified?
//        return true;

//    //don't notify subscribers.
//    return false;
//}

QList<Bacnet::CovSubscription> & CovSupport::covSubscriptions()
{
    return _subscriptions;
}

void CovSupport::propertyChanged(BacnetPropertyNS::Identifier propId, quint32 propArrayIdx, BacnetObject *notifyingObject, BacnetDeviceObject *deviceToNotify)
{
    Q_CHECK_PTR(notifyingObject);
    Q_CHECK_PTR(deviceToNotify);

    //this is not a property issuing COV notification.
    if (!covProperties().contains(propId))
        return;


    QList<Bacnet::CovSubscription> &subscriptions = covSubscriptions();
    QList<Bacnet::CovSubscription>::Iterator it = subscriptions.begin();
    QList<Bacnet::CovSubscription>::Iterator itEnd = subscriptions.end();

    int propertyValueIdx(-1);//index of changed property in covPropertiesValues list
    QList<PropertyValueShared> covPropertiesValues;
    //helper variable, to make covPropertiesValues lazy initialized.
    enum {
        NotChecked,
        Inform,
        DontInform } defaultIncrementState(NotChecked);
    Bacnet::Error error;

    for (; it != itEnd; ++it) {
        if (it->isCovObjectSubscription() || it->isCovPropertySubscription(propId, propArrayIdx)) {

            //property value is lazy initialized; being here means we surely need it.
            if (-1 == propertyValueIdx) {
                covPropertiesValues.append(PropertyValueShared(new PropertyValue(propId,
                                                                                 BacnetDataInterfaceShared(notifyingObject->propertyReadInstantly(propId, propArrayIdx, &error)),
                                                                                 propArrayIdx)));
                propertyValueIdx = covPropertiesValues.count() - 1;
                Q_ASSERT(propertyValueIdx >= 0);
                if (covPropertiesValues[propertyValueIdx].isNull() || covPropertiesValues[propertyValueIdx]->_value.isNull()) {
                    qDebug("%s : error while reading instantly value that changed %d", __PRETTY_FUNCTION__, propId);
                    return;
                }
            }

            CovRealIcnrementHandler *covHandler = it->covHandler();
            if (0 != covHandler) {
                //subscription has its own covIncrementHandler.
                Q_ASSERT(!it->isCovObjectSubscription()); //only property subscriptions are allowed to have their own covIncrements.
                covPropertiesValues[propertyValueIdx]->_value->accept(covHandler);
                if (!covHandler->isEqualWithinIncrement()) { //changed more than the increment. Notify subscriber!
                    deviceToNotify->propertyValueChanged(*it, notifyingObject, QList<PropertyValueShared>() << covPropertiesValues[propertyValueIdx]);
                }
            } else {
                //subscription uses common/default increment handler.
                if (NotChecked == defaultIncrementState) {
                    //check only once, if the default COVIncrement was exceeded.
                    covHandler = covIncrementHandler(propId);
                    //if we have registered covIncrementHandler, check if notification is to be sent.
                    if (0 != covHandler) {
                        covPropertiesValues[propertyValueIdx]->_value->accept(covHandler);
                        if (covHandler->isEqualWithinIncrement()) {//the value changed less than the increment, since last time. Don't inform all.
                            defaultIncrementState = DontInform;
                            continue;
                        }
                    }
                    //being here means, either no covHandler was declared (send notification on every change) or increment was exceeded.
                    defaultIncrementState = Inform;
                }

                if (it->isCovObjectSubscription()) {
                    /**
                        \note Here we lazy initialize the covPropertiesValues list. If there is no object-subscriber, there is no need to initialize it.
                        However if there is, fill it only once!
                        \note If the list is empty (and we know that covProperties() is not empty, since we are here), the list is uninitialized yet.
                    */
                    if (covPropertiesValues.isEmpty()) {
                        foreach (BacnetPropertyNS::Identifier id, covProperties()) {
                            if (id != propId) {//the property with propId is already added.
                                const quint32 arrayIdx = Bacnet::ArrayIndexNotPresent;
                                BacnetDataInterfaceShared propertyValueData(notifyingObject->propertyReadInstantly(id, arrayIdx, &error));
                                if (propertyValueData.isNull()) {
                                    qDebug("%s : cannot read covPropertyValue for values list %d", __PRETTY_FUNCTION__, id);
                                    return;//all the allocated data will be freed by shared pointers.
                                }
                                covPropertiesValues.append(PropertyValueShared(new PropertyValue(id, propertyValueData, arrayIdx)));
                            }
                        }
                    }

                    //covPropertiesValues is already initialized here.
                    deviceToNotify->propertyValueChanged(*it, notifyingObject, covPropertiesValues);
                } else {
                    deviceToNotify->propertyValueChanged(*it, notifyingObject, QList<PropertyValueShared>() << covPropertiesValues[propertyValueIdx]);
                }
            }
        }
    }
}

bool CovSupport::timeout(int timePassed_s)
{
    QList<Bacnet::CovSubscription>::Iterator it = _subscriptions.begin();
    QList<Bacnet::CovSubscription>::Iterator endIt = _subscriptions.end();

    bool changeOccured(false);

    while (it != endIt) {
        if (it->isSubscriptionTimeVariant()) {
            if (it->timeLeft() > timePassed_s)
                it->updateTimeLeft(timePassed_s);
            else {
                it = _subscriptions.erase(it);
                changeOccured = true;
                continue;//to omit ++it
            }
        }
        ++it;
    }

    //check if time dependency is still required
    if (changeOccured) {
        return hasTimeVariantSubscription();
    }

    return true;
}

bool CovSupport::hasTimeVariantSubscription()
{
    QList<Bacnet::CovSubscription>::Iterator it = _subscriptions.begin();
    QList<Bacnet::CovSubscription>::Iterator endIt = _subscriptions.end();

    for (; it != endIt; ++it) {
        if (it->isSubscriptionTimeVariant()) {
            return true;
        }
    }

    return false;
}

void CovSupport::updateWithTimeHandlerHelper()
{
    if (hasTimeVariantSubscription())
        CovSubscriptionsTimeHandler::instance()->addCovSupporter(this);
    else
        CovSubscriptionsTimeHandler::instance()->rmCovSupporter(this);
}

/////////////////////////////////////////////////////////////
//template <typename T, class T2>
//ConcreteCovSupport<T, T2>::ConcreteCovSupport(T covIncrement, T lastValue):
//        _lastValue(lastValue)
//{
//    QVariant value(covIncrement);
//    _covIncrement.setInternal(value);
//}

//template <typename T, class T2>
//void ConcreteCovSupport<T, T2>::setCovIncrement(T covIncrement)
//{
//    QVariant value(covIncrement);
//    _covIncrement.setInternal(value);
//}

//template <typename T, class T2>
//bool ConcreteCovSupport<T, T2>::hasChangeOccured(QVariant &actualValue)
//{
//    bool ok;
//    T covIncrement = toT<T>(_covIncrement.toInternal(), &ok);
//    Q_ASSERT(ok);
//    if (!ok) return true; //if something wrong happens during conversion, treat it that value has changed!
//    T actValue = toT<T>(actualValue, &ok);
//    Q_ASSERT(ok);
//    if (!ok) return true;//same as above
//    actValue -= _lastValue;
//    return qAbs(actValue) >= covIncrement;
//}

//template <typename T, class T2>
//void ConcreteCovSupport<T, T2>::setCovIncrement(BacnetDataInterface *newIncrement)
//{
//    Q_ASSERT(newIncrement);
//    QVariant value = newIncrement->toInternal();
//    _covIncrement.setInternal(value);
//}

//template <typename T, class T2>
//BacnetDataInterface *ConcreteCovSupport<T, T2>::covIncrement()
//{
//    return &_covIncrement;
//}

////define those that will be used.
//template class ConcreteCovSupport<float, Bacnet::Real>;
//template class ConcreteCovSupport<double, Bacnet::Double>;
