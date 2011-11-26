#ifndef BACNET_INTERNALIAMSERVICEHANDLER_H
#define BACNET_INTERNALIAMSERVICEHANDLER_H

#include "internalunconfirmedrequesthandler.h"
#include "iamservicedata.h"

namespace Bacnet {

class BacnetApplicationLayerHandler;

class InternalIAmServiceHandler:
        public InternalUnconfirmedRequestHandler
{
public:
    InternalIAmServiceHandler(BacnetApplicationLayerHandler *appLayer, BacnetAddress requester);

public://overridden from InternalRequestHandler - InternalUnconfirmedRequestHandler
    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device);
    virtual bool isFinished();
    virtual void finalize(bool *deleteAfter);
    virtual bool execute();
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

private:
    IAmServiceData _data;
    BacnetApplicationLayerHandler *_appLayer;
    BacnetAddress _requester;
};

} // namespace Bacnet

#endif // BACNET_INTERNALIAMSERVICEHANDLER_H
