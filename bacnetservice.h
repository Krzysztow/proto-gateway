#ifndef BACNETSERVICE_H
#define BACNETSERVICE_H

/** This class is used as a base class of both confirmed and unconfirmed services.
    In case of confirmed - takeResponse() should return nonzero pointer to the response
    that was created after the service request was successfully returned. In case of error
    it shouldn't create the response and set an error.
    For unconfirmed services only error should be manipulated (if something is wrong). Get res-
    ponse should always return 0 which will result in simple ACK response.

  */

#include <QtCore>

namespace Bacnet {class Error;}
class BacnetDeviceObject;
class BacnetObject;
class BacnetService
{
public:
    virtual ~BacnetService();

    //! \warning The response is created only once. Caller takes ownership over the response.
    virtual BacnetService *takeResponse() = 0;
    virtual bool asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object) = 0;

    virtual qint32 execute(BacnetDeviceObject *device) = 0;
    virtual bool isReady() = 0;
    virtual bool hasError() = 0;
    virtual Bacnet::Error &error() = 0;

    virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
};

#endif // BACNETSERVICE_H
