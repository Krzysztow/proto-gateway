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
    _settingsFlags(0)
{
    Q_ASSERT(sizeof (FlagsContainer) >= sizeof (CovReadStrategyFlags));
    setIsConfirmed(isConfirmed);
    setTimeDependantReadingWhenError(readTimelyWhenError);
}

bool Bacnet::CovReadStrategy::isPeriodic()
{
    return true;
}

void Bacnet::CovReadStrategy::setIsConfirmed(bool isConfirmed)
{
    if (isConfirmed) {
        _settingsFlags |= Flag_CovConfirmed;
        _settingsFlags &= ~(Flag_CovUnconfirmed);
    } else {
        _settingsFlags |= Flag_CovUnconfirmed;
        _settingsFlags &= ~(Flag_CovConfirmed);
    }
}

bool Bacnet::CovReadStrategy::timePassed(int timePassed_ms)
{
    _timeToAction_ms -= timePassed_ms;
    return (_timeToAction_ms < 0);
}

void Bacnet::CovReadStrategy::doAction(Bacnet::ExternalPropertyMapping *propertyMapping, Bacnet::ExternalObjectsHandler *externalHandler)
{
    Q_ASSERT(propertyMapping);
    Q_ASSERT(externalHandler);
    if (!hasError())
        externalHandler->makeCovSubscription(propertyMapping, isConfirmed(), _resubscriptionInterval_ms/1000, this);
    else if (hasTimeDependantReadingWhenError())
        externalHandler->readProperty(propertyMapping, false);
}

bool Bacnet::CovReadStrategy::isValueReady()
{
    if (isSubscriptionInitiated())
        return true;
    else
        return false;
}

void Bacnet::CovReadStrategy::setInterval(int interval_ms)
{
    _resubscriptionInterval_ms = interval_ms;
}

int Bacnet::CovReadStrategy::interval()
{
    return _resubscriptionInterval_ms;
}

void Bacnet::CovReadStrategy::setTimeDependantReadingWhenError(bool set)
{
    if (set)
        _settingsFlags = _settingsFlags | Flag_CovTimeReadEnable;
    else
        _settingsFlags &= ~(Flag_CovTimeReadEnable);
}

void CovReadStrategy::setSubscriptionInitiated(bool success, bool setError)
{
    if (success) {
        _settingsFlags |= Flag_CovInitialized;
        setHasError(false);
    } else {
        _settingsFlags &= ~(Flag_CovInitialized);
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

