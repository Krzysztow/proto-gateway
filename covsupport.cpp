#include "covsupport.h"

#include "error.h"
#include "subscribecovservicedata.h"
#include "bacnetaddress.h"
#include "covincrementhandlers.h"

using namespace Bacnet;

CovSupport::~CovSupport()
{
    delete _incrementHandler;
    _incrementHandler = 0;
}

void CovSupport::addOrUpdateCovSubscription(Bacnet::SubscribeCOVServiceData &covData, BacnetAddress &requester, Bacnet::Error *error)
{
    Q_CHECK_PTR(error);

    //is property supported?
    if (!covProperties().contains(covData._propReference->propIdentifier())) {
        qDebug("%s : Property is not cov supported", __PRETTY_FUNCTION__);
        error->setError(BacnetError::ClassObject, BacnetError::CodeNotCovProperty);
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
        _subscriptions.append(Bacnet::CovSubscription(covData, requester));
        subscription = &(_subscriptions.last());
    }

    Q_ASSERT(0 != subscription);
    if (subscription == 0) {
        error->setError(BacnetError::ClassResources, BacnetError::CodeOther);
        return;
    }

    subscription->update(covData._lifetime, covData.takeCovIncrement());
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
    }
}

void CovSupport::setDefaultIncrementHandler(CovRealIcnrementHandler *covIncrement)
{
    delete _incrementHandler;
    _incrementHandler = covIncrement;
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
