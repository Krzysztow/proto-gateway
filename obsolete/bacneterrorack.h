#ifndef BacnetErrorAck_H
#define BacnetErrorAck_H

#include "bacnetservice.h"
#include "bacnetcommon.h"
#include "error.h"

class BacnetDeviceObject;
class BacnetErrorAck:
        public BacnetService
{
public:
    BacnetErrorAck(Bacnet::Error error);
    ~BacnetErrorAck();

    //! \warning The response is created only once. Caller takes ownership over the response.
    virtual BacnetService *takeResponse() {return 0;}
    virtual bool asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object) {return true;}

    virtual bool isReady() {return true;}
    virtual bool hasError() {return false;}
    virtual Bacnet::Error &error() {}
    virtual qint32 execute(BacnetDeviceObject *device) {return -1;}

private:
    Bacnet::Error _error;
};

#endif // BacnetErrorAck_H
