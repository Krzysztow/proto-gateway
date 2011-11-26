#ifndef BACNET_INTERNALIHAVESERVICEHANDLER_H
#define BACNET_INTERNALIHAVESERVICEHANDLER_H

#include "internalunconfirmedrequesthandler.h"
#include "ihaveservicedata.h"

namespace Bacnet {

class BacnetApplicationLayerHandler;

class InternalIHaveServiceHandler :
        public InternalUnconfirmedRequestHandler
{
public:
    InternalIHaveServiceHandler(BacnetAddress &requester,
                                BacnetApplicationLayerHandler *appLayer);

public://overridden from InternalRequestHandler - InternalUnconfirmedRequestHandler
    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device);
    virtual bool isFinished();
    virtual void finalize(bool *deleteAfter);
    virtual bool execute();
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

private:
    IHaveServiceData _data;
    BacnetAddress _requester;
    BacnetApplicationLayerHandler *_appLayer;
};

} // namespace Bacnet

#endif // BACNET_INTERNALIHAVESERVICEHANDLER_H
