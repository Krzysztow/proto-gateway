#ifndef INTERNALRPREQUESTHANDLER_H
#define INTERNALRPREQUESTHANDLER_H

#include "internalconfirmedrequesthandler.h"
#include "readpropertyservicedata.h"
#include "bacnetdata.h"

namespace Bacnet {

class BacnetApplicationLayerHandler;

    class InternalRPRequestHandler:
        public ::InternalConfirmedRequestHandler
    {
    public:
        InternalRPRequestHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination,
                                 BacnetDeviceObject *device,
                                 BacnetApplicationLayerHandler *appLayer);
        virtual ~InternalRPRequestHandler();

    public://! overriden from InternalWhoIsRequestHandler, but \todo hsould be declaredin InternalRequestHandler.
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
        bool finishReading_helper(BacnetObject *readObject, int resultCode, BacnetDataInterfaceShared *value = 0);

    private:
        BacnetDeviceObject *_device;
        BacnetApplicationLayerHandler *_appLayer;

        ReadPropertyServiceData _data;
        int _asynchId;
        Error _error;
        BacnetServiceData *_response;
    };

}

#endif // INTERNALRPREQUESTHANDLER_H
