#ifndef BACNETCOVSUPPORT_H
#define BACNETCOVSUPPORT_H

#include "bacnetconstructeddata.h"
#include "covincrementhandlers.h"

namespace Bacnet {

class SubscribeCOVServiceData;

class CovSubscription :
    public BacnetDataInterface
{
public://overriden BacnetDataInterface methonds
    virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
    virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
    virtual qint32 fromRaw(BacnetTagParser &parser);
    virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

    virtual bool setInternal(QVariant &value);
    virtual QVariant toInternal();

    virtual DataType::DataType typeId();

    static const quint32 SubscriptionNotTimeVariant = 0;

public:
    CovSubscription(SubscribeCOVServiceData &data, BacnetAddress &address);
    ~CovSubscription();

    bool compareSubscriptions(CovSubscription &other);
    bool compareParametrs(BacnetAddress &recipientAddress, quint32 recipientProcessId,
                          ObjectIdentifier &objectId, BacnetProperty::Identifier propertyId, quint32 propertyArrayIdx = ArrayIndexNotPresent);
    //! \note updates values such as _timeLeft and _covIncrement. It takes ownership over covRealIncrementHandler. By doing so it deletes it's own one.
    void update(quint32 lifetime, CovRealIcnrementHandler *covIncrement);

    //! Returns true, if the subscription is of object (not property) type.
    bool isCovObjectSubscription() { return (_monitoredPropertyRef.propId() == BacnetProperty::UndefinedProperty); }
    bool isCovPropertySubscription(BacnetProperty::Identifier propId, quint32 propertyArrayIdx = ArrayIndexNotPresent);
    CovRealIcnrementHandler *covHandler() {return _covIncrement;}

    bool isIssueConfirmedNotifications() {return _issueConfNotification;}

    bool recipientHasAddress() {return _recipientProcess.recipientHasAddress();}
    const Address *recipientAddress() const {return _recipientProcess.recipientAddress();}
    const ObjectIdentifier *recipientObjId() const {return _recipientProcess.recipientObjId();}

    bool isSubscriptionTimeVariant() {return (_timeLeft != SubscriptionNotTimeVariant);}
    quint32 timeLeft() {return _timeLeft;}
    void updateTimeLeft(quint32 timePassed) {_timeLeft -= timePassed;}

public:
    RecipientProcess _recipientProcess;
    ObjectPropertyReference _monitoredPropertyRef;
    bool _issueConfNotification;
    //BacnetAddress _subscriberAddress;
    quint32 _timeLeft;
    CovRealIcnrementHandler *_covIncrement;
};

class BacnetCovSupport
{
public:
    BacnetCovSupport();
};

}

#endif // BACNETCOVSUPPORT_H
