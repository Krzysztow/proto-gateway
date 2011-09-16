#ifndef WHOISSERVICEHANDLER_H
#define WHOISSERVICEHANDLER_H

#include "whoisservicedata.h"
#include "asynchronousactionhandler.h"


class InternalObjectsHandler;
class BacnetDeviceObject;

namespace Bacnet {

    class BacnetTSM2;
    class WhoIsServiceHandler:
            InternalRequestHandler
    {
    public:
        WhoIsServiceHandler(InternalObjectsHandler *internalHndlr, BacnetTSM2 *tsm, BacnetDeviceObject *device = 0);

        qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public://Overriden from
        virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
        virtual bool isFinished();
        virtual void finalize(bool *deleteAfter);
        virtual QList<int> execute();

    private:
        WhoIsServiceData _data;

        InternalObjectsHandler *_internalHndlr;
        BacnetTSM2 *_tsm;
        BacnetDeviceObject *_device;
    };
}
#endif // WHOISSERVICEHANDLER_H
