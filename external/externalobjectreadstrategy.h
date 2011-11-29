#ifndef BACNET_EXTERNALOBJECTREADSTRATEGY_H
#define BACNET_EXTERNALOBJECTREADSTRATEGY_H

#include <QtCore>
#include "externaltimedepjob.h"

namespace Bacnet {

class ExternalObjectsHandler;
class ExternalPropertyMapping;

class ExternalObjectReadStrategy:
        public ExternalTimeDepJob
{
public://time-dependant behaviour / implemented ExternalTimeDepJob methods
    //! returns true, if the action is periodic
    virtual bool isPeriodic();
    //! returns true, if action is to be executed
    virtual bool timePassed(int timePassed_ms);
    //! executes action
    virtual void doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler);

    enum FinishStatus {
        FinishedOk,
        FinishedWithError,
        FinishedCritical
    };

    virtual void actionFinished(FinishStatus finishStatus);

public://methods considering property readyness to be read
    virtual int readProperty(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler, bool generateAsynchId = false);
};

/**
  This strategy assumes that it is always necessary to read the property, no matter what.
  */
typedef ExternalObjectReadStrategy SimpleReadStrategy;

/**
    This strategy works similarily to the SimpleReadStrategy. The difference is that it polls for a data from time to time.
  */

class SimpleWithTimeReadStrategy:
        public ExternalObjectReadStrategy
{
public:
    SimpleWithTimeReadStrategy(int interval_ms = DefaultInterval);

public://time-dependant behaviour
    //! returns true, if the action is periodic
    virtual bool isPeriodic();
    //! returns true, if action is to be executed
    virtual bool timePassed(int timePassed_ms);
    //! executes action
    virtual void doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler);

    virtual void actionFinished(FinishStatus finishStatus);

    //readProperty() is taken from ExternalObjectReadStrategy

public:
    void setInterval(int interval_ms);
    int interval();

private:
    static const int DefaultInterval = 10000;
    int _interval_ms;
    int _timeToAction_ms;
};

/**
    This strategy supports COV notifications. That means, if the subscription is successful it allows for instant read.
    If subscription was aborted and we have hints we have no chances to subscribe it can behave like SimpleWithTimeReadStrategy
    with covResubscription interval (but the flag has to be set). Otherwise it acts as if it was SimpleReadStrategy and resubscriptions
    are tried each covResubscription interval.
  */

class CovNotificationRequestData;

class CovReadStrategy:
        public ExternalObjectReadStrategy
{
public:
    static const int SubscrInfiniteTime = 0;
    static const int DefaultInterval = 10000;
public:
    CovReadStrategy(int resubscriptionInterval_ms = DefaultInterval, bool isConfirmed = false, bool readTimelyWhenError = false);

    void setTimeDependantReadingWhenError(bool set);
    inline bool hasTimeDependantReadingWhenError() {return _settingsFlags & Flag_CovTimeReadEnable;}

    void setIsConfirmed(bool isConfirmed);
    inline bool isConfirmed() {return _settingsFlags & Flag_CovConfirmed;}

    void setHasIncrement(bool hasIncrement, float incrValue = 0);
    inline bool hasIncrement() {return _settingsFlags & Flag_CovHasIncrement;}
    inline float incrementValue() {return _increment;}

    inline bool isSubscriptionInitiated() {return _settingsFlags & Flag_CovInitialized;}
    void setSubscriptionInitiated(bool success, int subscriptionId, bool setError = false);
    int subscriptionProcId();

    void notificationReceived(CovNotificationRequestData &data, bool confirmed);

public://time-dependant behaviour
    //! returns true, if the action is periodic
    virtual bool isPeriodic();
    //! returns true, if action is to be executed
    virtual bool timePassed(int timePassed_ms);
    //! executes action
    virtual void doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler);

    virtual void actionFinished(FinishStatus finishStatus);

public:
    virtual int readProperty(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler, bool generateAsynchId);

public:
    void setInterval(int interval_ms);
    int interval();

    enum CovReadStrategyFlags {
        Flag_CovConfirmed       = 0x01,
        Flag_CovUnconfirmed     = 0x02,
        Flag_CovInitialized     = 0x04,
        Flag_CovError           = 0x08,
        Flag_CovTimeReadEnable  = 0x10,
        Flag_CovHasIncrement    = 0x20,

        CHECK_SIZE
    };

private:

    void setHasError(bool hasError);
    inline bool hasError() {return _settingsFlags & Flag_CovError;}

private:
    int _resubscriptionInterval_ms;
    int _timeToAction_ms;
    int _subscriptionId;
    float _increment;
    typedef quint8 FlagsContainer;
    FlagsContainer _settingsFlags;
};

} // namespace Bacnet

#endif // BACNET_EXTERNALOBJECTREADSTRATEGY_H
