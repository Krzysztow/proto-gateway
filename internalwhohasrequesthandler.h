#ifndef INTERNALWHOHASREQUESTHANDLER_H
#define INTERNALWHOHASREQUESTHANDLER_H

#include "internalunconfirmedrequesthandler.h"
#include "whohasservicedata.h"

namespace Bacnet {

    class InternalWhoHasRequestHandler:
        public ::InternalUnconfirmedRequestHandler
    {
    public:
        InternalWhoHasRequestHandler(BacnetAddress &requester,
                                     Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                     InternalObjectsHandler *internalHandler);
        virtual ~InternalWhoHasRequestHandler();

    public://! overriden from InternalWhoIsRequestHandler, but \todo hsould be declaredin InternalRequestHandler.
        virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

    public://overriden InternalRequestHandler methods.
        virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
        virtual bool isFinished();
        virtual void finalize(bool *deleteAfter);

        bool execute();

    private:
        Bacnet::BacnetTSM2 *_tsm;
        BacnetDeviceObject *_device;
        BacnetAddress _requester;
        InternalObjectsHandler *_internalHandler;

        WhoHasServiceData _data;
    };

}
#endif // INTERNALWHOHASREQUESTHANDLER_H
