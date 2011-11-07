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
    virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength) = 0;
    virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber) = 0;
    virtual qint32 fromRaw(BacnetTagParser &parser) = 0;
    virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum) = 0;

    virtual bool setInternal(QVariant &value) = 0;
    virtual QVariant toInternal() = 0;

    virtual DataType::DataType typeId() = 0;

public:
    CovSubscription(SubscribeCOVServiceData &data, BacnetAddress &address);
    bool compareSubscriptions(CovSubscription &other);


public:
    Recipient _recipient;
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
