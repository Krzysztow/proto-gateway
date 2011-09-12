#ifndef BACNETREADPROPERTYACK_H
#define BACNETREADPROPERTYACK_H

#include "bacnetservice.h"
#include "bacnetcommon.h"
#include "readpropertyservicedata.h"

namespace Bacnet {
    class BacnetDataInterface;
}

class BacnetReadPropertyAck:
        public BacnetService
{
public:
    BacnetReadPropertyAck();
    ~BacnetReadPropertyAck();
    virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
    virtual qint32 fromRaw(quint8 *startPtr, quint16 buffLength);

    //remove these stupid functions
    virtual BacnetService *takeResponse() {return 0;}
    virtual bool asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object) {return true;}

    virtual bool isReady() {return true;}
    virtual bool hasError() {return false;}
    virtual Bacnet::Error &error() {}
    virtual qint32 execute(BacnetDeviceObject *device) {return -1;}

public:
    void setData(Bacnet::BacnetDataInterface *data, Bacnet::ReadPropertyServiceData &ackReadPrptyData);
    Bacnet::BacnetDataInterface *data();
    Bacnet::ReadPropertyServiceData &value();


    //data that has been encoded in the ack
    Bacnet::ReadPropertyServiceData _value;
    Bacnet::BacnetDataInterface *_data;
};

#endif // BACNETREADPROPERTYACK_H
