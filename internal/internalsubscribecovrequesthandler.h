#ifndef INTERNALSUBSCRIBECOVREQUESTHANDLER_H
#define INTERNALSUBSCRIBECOVREQUESTHANDLER_H

#include "internalconfirmedrequesthandler.h"
#include "subscribecovservicedata.h"
#include "error.h"

namespace Bacnet {

class BacnetApplicationLayerHandler;

    class InternalSubscribeCOVRequestHandler:
        public ::InternalConfirmedRequestHandler
    {
    public:
        InternalSubscribeCOVRequestHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination,
                                           Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                           BacnetApplicationLayerHandler *appLayer);
        virtual ~InternalSubscribeCOVRequestHandler();

    public://! overriden from InternalSubscribeCOVRequestHandler, but \todo hsould be declaredin InternalRequestHandler.
        virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

    public://overriden InternalRequestHandler methods.
        bool execute();
        virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
        virtual bool isFinished();
        virtual void finalize(bool *deleteAfter);

    public://overriden InternalRequestHandler methods.
        virtual bool hasError();
        virtual Bacnet::Error &error();
        virtual Bacnet::BacnetServiceData *takeResponseData();

    private:
        Bacnet::BacnetTSM2 *_tsm;
        BacnetDeviceObject *_device;
        BacnetApplicationLayerHandler *_appLayer;

        SubscribeCOVServiceData _data;
        Error _error;
    };

}

#endif // INTERNALSUBSCRIBECOVREQUESTHANDLER_H
