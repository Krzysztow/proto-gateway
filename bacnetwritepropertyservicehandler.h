#ifndef BACNETWRITEPROPERTYSERVICEHANDLER_H
#define BACNETWRITEPROPERTYSERVICEHANDLER_H

#include "bacnetconfirmedservicehandler.h"

class PropertySubject;

namespace Bacnet {

    class ExternalObjectsHandler;
    class WritePropertyServiceData;

    class BacnetWritePropertyServiceHandler:
            public BacnetConfirmedServiceHandler
    {
    public:
        BacnetWritePropertyServiceHandler(WritePropertyServiceData *wData, ExternalObjectsHandler *respHandler);
        virtual ~BacnetWritePropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual void handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action);
        virtual void handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action);
        virtual void handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action);
        virtual quint32 handleTimeout(ActionToExecute *action);

    private:
        //BACnet specific
        WritePropertyServiceData *_wData;
        quint8 sendTryOuts;
        //CDM handling
        ExternalObjectsHandler *_responseHandler;
    };

}


#endif // BACNETWRITEPROPERTYSERVICEHANDLER_H
