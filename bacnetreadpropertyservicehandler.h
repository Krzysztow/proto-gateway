#ifndef BACNETREADPROPERTYSERVICEHANDLER_H
#define BACNETREADPROPERTYSERVICEHANDLER_H

#include "bacnetconfirmedservicehandler.h"


class BacnetReadPropertyService;
class PropertySubject;

namespace Bacnet {

    class ExternalObjectsHandler;

    class BacnetReadPropertyServiceHandler:
            public BacnetConfirmedServiceHandler
    {
    public:
        BacnetReadPropertyServiceHandler(BacnetReadPropertyService *rpData, ExternalObjectsHandler *respHandler);
        virtual ~BacnetReadPropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual void handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action);
        virtual void handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action);
        virtual void handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action);
        virtual quint32 handleTimeout(ActionToExecute *action);

    private:
        //BACnet specific
        BacnetReadPropertyService *_rpData;
        quint8 sendTryOuts;
        //CDM handling
        ExternalObjectsHandler *_responseHandler;
    };

}

#endif // BACNETREADPROPERTYSERVICEHANDLER_H
