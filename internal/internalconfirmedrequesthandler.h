#ifndef ASYNCHRONOUSRPHANDLER_H
#define ASYNCHRONOUSRPHANDLER_H

#include "internalrequesthandler.h"
#include "bacnetaddress.h"
#include "error.h"
#include "bacnetservicedata.h"

namespace Bacnet {
    class BacnetApplicationLayerHandler;
    class ExternalObjectsHandler;
    class InternalObjectsHandler;
}
class BacnetConfirmedRequestData;

class InternalConfirmedRequestHandler:
        public InternalRequestHandler
{
public:
    InternalConfirmedRequestHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination);
    virtual ~InternalConfirmedRequestHandler();

public://methods overridden from InternalRequestHandler
    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device) = 0;
    virtual bool isFinished() = 0;
    virtual void finalize(bool *deleteAfter) = 0;
    virtual bool execute() = 0;
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length) = 0;

public:
//    void setConfirmedData(BacnetConfirmedRequestData *reqData);
//    void setAddresses(BacnetAddress &requester, BacnetAddress &destination);

public:
    virtual bool hasError() = 0;
    virtual Bacnet::Error &error() = 0;
    virtual Bacnet::BacnetServiceData *takeResponseData() = 0;

public:
    void finalizeInstant(Bacnet::BacnetApplicationLayerHandler *appLayer);

protected:
    BacnetConfirmedRequestData *_reqData;
    BacnetAddress _requester;
    BacnetAddress _destination;
};

#endif // ASYNCHRONOUSRPHANDLER_H
