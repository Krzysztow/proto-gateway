#ifndef INTERNALWHOISREQUESTHANDLER_H
#define INTERNALWHOISREQUESTHANDLER_H

#include "internalunconfirmedrequesthandler.h"
#include "whoisservicedata.h"

namespace Bacnet {

class BacnetApplicationLayerHandler;

    class InternalWhoIsRequestHandler:
        public ::InternalUnconfirmedRequestHandler
    {
    public:
        InternalWhoIsRequestHandler(BacnetAddress &requester,
                                    Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                    BacnetApplicationLayerHandler *appLayer);
        virtual ~InternalWhoIsRequestHandler();

    public://! overriden from InternalWhoIsRequestHandler, but \todo hsould be declaredin InternalRequestHandler.
        virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

    public://overriden InternalRequestHandler methods.
        virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
        virtual bool isFinished();
        virtual void finalize(bool *deleteAfter);

        virtual bool execute();

    private:
        Bacnet::BacnetTSM2 *_tsm;
        BacnetDeviceObject *_device;
        BacnetAddress _requester;
        BacnetApplicationLayerHandler *_appLayer;

        WhoIsServiceData _data;
    };

}

#endif // INTERNALWHOISREQUESTHANDLER_H
