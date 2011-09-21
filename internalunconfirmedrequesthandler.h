#ifndef INTERNALUNCONFIRMEDREQUESTHANDLER_H
#define INTERNALUNCONFIRMEDREQUESTHANDLER_H

#include "internalconfirmedrequesthandler.h"
#include "bacnetaddress.h"

/** We stick to this upper class
  */

namespace Bacnet {
    class BacnetTSM2;
    class ExternalObjectsHandler;
};
class InternalObjectsHandler;
class BacnetUnconfirmedRequestData;
class BacnetService;

class InternalUnconfirmedRequestHandler:
        public InternalRequestHandler
{
public:
    InternalUnconfirmedRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                      InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler);
    virtual ~InternalUnconfirmedRequestHandler();

    //! \todo move this function higher, to InternalRequestHandler!
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length) = 0;

public:
    void setUnconfirmedData(BacnetUnconfirmedRequestData *reqData);
    void setAddresses(BacnetAddress &requester, BacnetAddress &destination);

protected:
    BacnetUnconfirmedRequestData *_reqData;
    BacnetAddress _requester;
    BacnetAddress _destination;

//private:
//    Bacnet::BacnetTSM2 *_tsm;
//    BacnetService *_service;
//    BacnetDeviceObject *_device;
//    InternalObjectsHandler *_internalHandler;
//    ExternalObjectsHandler *_externalHandler;
};

#endif // INTERNALUNCONFIRMEDREQUESTHANDLER_H
