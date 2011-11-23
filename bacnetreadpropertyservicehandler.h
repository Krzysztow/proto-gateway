#ifndef BACNETREADPROPERTYSERVICEHANDLER_H
#define BACNETREADPROPERTYSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"


class PropertySubject;

namespace Bacnet {

    class ExternalObjectsHandler;
    class ReadPropertyServiceData;

    class ReadPropertyServiceHandler:
            public ExternalConfirmedServiceHandler
    {
    public:
        ReadPropertyServiceHandler(ReadPropertyServiceData *rpData, ExternalObjectsHandler *respHandler);
        virtual ~ReadPropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual void handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action);
        virtual void handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action);
        virtual void handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action);
        virtual quint32 handleTimeout(ActionToExecute *action);

    private:
        //BACnet specific
        ReadPropertyServiceData *_rpData;
        quint8 sendTryOuts;
        //CDM handling
        ExternalObjectsHandler *_responseHandler;
    };

}

#endif // BACNETREADPROPERTYSERVICEHANDLER_H
