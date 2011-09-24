#ifndef INTERNALSUBSCRIBECOVREQUESTHANDLER_H
#define INTERNALSUBSCRIBECOVREQUESTHANDLER_H

#include "internalconfirmedrequesthandler.h"
#include "subscribecovservicedata.h"
#include "error.h"

namespace Bacnet {

    class InternalSubscribeCOVRequestHandler:
        public ::InternalConfirmedRequestHandler
    {
    public:
        InternalSubscribeCOVRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                    InternalObjectsHandler *internalHandler, ExternalObjectsHandler *externalHandler);
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
        InternalObjectsHandler *_internalHandler;
        ExternalObjectsHandler *_externalHandler;

        SubscribeCOVServiceData _data;
        Error _error;
    };

}

#endif // INTERNALSUBSCRIBECOVREQUESTHANDLER_H
