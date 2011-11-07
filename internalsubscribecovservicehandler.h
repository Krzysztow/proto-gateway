#ifndef INTERNALSUBSCRIBECOVSERVICEHANDLER_H
#define INTERNALSUBSCRIBECOVSERVICEHANDLER_H

#include "internalconfirmedrequesthandler.h"
#include "subscribecovservicedata.h"

namespace Bacnet {

class InternalSubscribeCovServiceHanlder:
        public ::InternalConfirmedRequestHandler
{
public:
    InternalSubscribeCovServiceHanlder(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                     InternalObjectsHandler *internalHandler, ExternalObjectsHandler *externalHandler);
    virtual ~InternalSubscribeCovServiceHanlder();

public://! overriden InternalConfirmedRequestHandler method/should be in InternalRequestHandler.
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

public://overriden InternalRequestHandler methods.
    virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
    virtual bool isFinished();
    virtual void finalize(bool *deleteAfter);
    virtual bool execute();

public://overriden InternalConfirmedRequestHandler methods.
    virtual bool hasError();
    virtual Error &error();
    virtual Bacnet::BacnetServiceData *takeResponseData();

private:
    Bacnet::BacnetTSM2 *_tsm;
    BacnetDeviceObject *_device;
    InternalObjectsHandler *_internalHandler;
    ExternalObjectsHandler *_externalHandler;

    SubscribeCOVServiceData _data;
    Error _error;
};

}

#endif //INTERNALSUBSCRIBECOVSERVICEHANDLER_H
