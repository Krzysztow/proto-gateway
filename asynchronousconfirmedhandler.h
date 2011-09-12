#ifndef ASYNCHRONOUSRPHANDLER_H
#define ASYNCHRONOUSRPHANDLER_H

#include "asynchronousactionhandler.h"
#include "bacnetaddress.h"

namespace Bacnet {class BacnetTSM2;};
class BacnetConfirmedRequestData;
class ReadPropertyServiceHandler;
class BacnetService;
class BacnetPciData;

class InternalConfirmedRequestHandler:
        public InternalRequestHandler
{
public:
    InternalConfirmedRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device);
    ~InternalConfirmedRequestHandler();

    void setRequester(BacnetAddress &address);
    void setRequestData(BacnetConfirmedRequestData *reqData);
    void setService(BacnetService *service);

public://overwritten from AsynchronousActionHandler
    virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
    virtual bool isFinished();
    virtual void finalize(bool *deleteAfter);

public:
    static void finalizeInstant(BacnetAddress &address, Bacnet::BacnetTSM2 *tsm, BacnetConfirmedRequestData *requestPci, BacnetService *requestService);

private:
    Bacnet::BacnetTSM2 *_tsm;
    BacnetAddress _requester;
    BacnetConfirmedRequestData *_reqData;
    BacnetService *_service;
    BacnetDeviceObject *_device;
};

#endif // ASYNCHRONOUSRPHANDLER_H
