#include "externalobjectreadstrategy.h"

using namespace Bacnet;

////////////////////////////////////////////////////
///////////ExternalObjectReadStrategy///////////////
////////////////////////////////////////////////////

bool ExternalObjectReadStrategy::isPeriodic()
{
    return false;
}

void ExternalObjectReadStrategy::doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler)
{
    Q_UNUSED(propertyMapping);
    Q_UNUSED(externalHandler);
    Q_ASSERT(false);
}

bool ExternalObjectReadStrategy::timePassed(int timePassed_ms)
{
    Q_UNUSED(timePassed_ms);
    Q_ASSERT(false);
    return false;
}

bool ExternalObjectReadStrategy::isValueReady()
{
    return false;
}

////////////////////////////////////////////////////
///////////////SimpleReadStrategy///////////////////
////////////////////////////////////////////////////



////////////////////////////////////////////////////
////////////SimpleWithTimeReadStrategy//////////////
////////////////////////////////////////////////////

#include "externalobjectshandler.h"
#include "externalpropertymapping.h"

SimpleWithTimeReadStrategy::SimpleWithTimeReadStrategy(int interval_ms):
    _interval_ms(interval_ms),
    _timeToAction_ms(0)
{
}

bool SimpleWithTimeReadStrategy::timePassed(int timePassed_ms)
{
    _timeToAction_ms -= timePassed_ms;
    return (_interval_ms < 0);
}

bool SimpleWithTimeReadStrategy::isPeriodic()
{
    return true;
}

void SimpleWithTimeReadStrategy::doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler)
{
    Q_CHECK_PTR(propertyMapping);
    Q_CHECK_PTR(propertyMapping->mappedProperty);

    externalHandler->readProperty(propertyMapping, false);
    _timeToAction_ms = _interval_ms;
}

void SimpleWithTimeReadStrategy::setInterval(int interval_ms)
{
    _interval_ms = interval_ms;
}

int SimpleWithTimeReadStrategy::interval()
{
    return _interval_ms;
}

////////////////////////////////////////////////////
////////////////CovReadStrategy/////////////////////
////////////////////////////////////////////////////


CovReadStrategy::CovReadStrategy(int resubscriptionInterval_ms, bool isConfirmed, bool readTimelyWhenError):
    _resubscriptionInterval_ms(resubscriptionInterval_ms),
    _timeToAction_ms(0),
    _subscriptionId(-1),
    _settingsFlags(0)
{
    Q_ASSERT( (1 << (int)log2(CHECK_SIZE)) > 8*sizeof(FlagsContainer) );
    setIsConfirmed(isConfirmed);
    setTimeDependantReadingWhenError(readTimelyWhenError);
}

bool CovReadStrategy::isPeriodic()
{
    return true;
}

void CovReadStrategy::setIsConfirmed(bool isConfirmed)
{
    if (isConfirmed) {
        _settingsFlags |= Flag_CovConfirmed;
        _settingsFlags &= ~(Flag_CovUnconfirmed);
    } else {
        _settingsFlags |= Flag_CovUnconfirmed;
        _settingsFlags &= ~(Flag_CovConfirmed);
    }
}

bool CovReadStrategy::timePassed(int timePassed_ms)
{
    _timeToAction_ms -= timePassed_ms;
    return (_timeToAction_ms < 0);
}

void CovReadStrategy::doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler)
{
    Q_ASSERT(propertyMapping);
    Q_ASSERT(externalHandler);
    if (!hasError()) {
        externalHandler->startCovSubscriptionProcess(propertyMapping, isConfirmed(), _resubscriptionInterval_ms/1000, this);
        qDebug("Subscribes cov with interval %d", _resubscriptionInterval_ms/1000);
        _timeToAction_ms = _resubscriptionInterval_ms;
    } else if (hasTimeDependantReadingWhenError())
        externalHandler->readProperty(propertyMapping, false);
}

bool CovReadStrategy::isValueReady()
{
    if (isSubscriptionInitiated())
        return true;
    else
        return false;
}

void CovReadStrategy::setInterval(int interval_ms)
{
    _resubscriptionInterval_ms = interval_ms;
}

int CovReadStrategy::interval()
{
    return _resubscriptionInterval_ms;
}

void CovReadStrategy::setTimeDependantReadingWhenError(bool set)
{
    if (set)
        _settingsFlags = _settingsFlags | Flag_CovTimeReadEnable;
    else
        _settingsFlags &= ~(Flag_CovTimeReadEnable);
}

void CovReadStrategy::setSubscriptionInitiated(bool success, int subscriptionId, bool setError)
{
    if (success) {
        _settingsFlags |= Flag_CovInitialized;
        setHasError(false);
        _subscriptionId = subscriptionId;
    } else {
        _settingsFlags &= ~(Flag_CovInitialized);
        _subscriptionId = -1;
        if (setError)
            setHasError(true);
    }
}

void CovReadStrategy::setHasError(bool hasError)
{
    if (hasError)
        _settingsFlags |= Flag_CovError;
    else
        _settingsFlags &= ~(Flag_CovError);
}

int CovReadStrategy::subscriptionProcId()
{
    return _subscriptionId;
}

void CovReadStrategy::setHasIncrement(bool hasIncrement, float incrValue)
{
    if (hasIncrement) {
        _increment = incrValue;
        _settingsFlags |= Flag_CovHasIncrement;
    } else
        _settingsFlags &= ~(Flag_CovHasIncrement);
}

void CovReadStrategy::notificationReceived(CovNotificationRequestData &data, bool confirmed)
{
    if (isConfirmed() != confirmed)
        qDebug("%s : Should be confirmed (%s), and is (%s)", __PRETTY_FUNCTION__, isConfirmed() ? "true" : "false", confirmed ? "true" : "false");
    if ( (SubscrInfiniteTime != data._timeLeft) && (_timeToAction_ms > 1000 * data._timeLeft) )//we thought we had more time -> make an update
        _timeToAction_ms = 1000 * data._timeLeft;
}

