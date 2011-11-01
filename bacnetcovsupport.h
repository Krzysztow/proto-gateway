#ifndef BACNETCOVSUPPORT_H
#define BACNETCOVSUPPORT_H

#include "bacnetconstructeddata.h"

namespace Bacnet {

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
    CovSubscription(Bacnet::SubscribeCOVServiceData &data, BacnetAddress &address);
    bool compareSubscriptions(Bacnet::SubscribeCOVServiceData &other);

public:

    PropertyReference _subscriberProcId;
    ObjectIdStruct _monitoredObjectId;
    bool _issueConfNotification;
    BacnetAddress _subscriberAddress;
    quint32 _timeLeft;
};

class BacnetCovSupport
{
public:
    BacnetCovSupport();
};

}

#endif // BACNETCOVSUPPORT_H
