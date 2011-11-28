#ifndef BACNETWRITEPROPERTYSERVICEHANDLER_H
#define BACNETWRITEPROPERTYSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"

class PropertySubject;

namespace Bacnet {

    class ExternalObjectsHandler;
    class WritePropertyServiceData;

    class BacnetWritePropertyServiceHandler:
            public ExternalConfirmedServiceHandler
    {
    public:
        BacnetWritePropertyServiceHandler(WritePropertyServiceData *wData, ExternalObjectsHandler *respHandler, Property *property, int asynchId);
        virtual ~BacnetWritePropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
        virtual ActionToExecute handleError(quint8 *errorPtr, quint16 length);
        virtual ActionToExecute handleReject(quint8 *abortPtr, quint16 length);
        virtual ActionToExecute handleAbort(quint8 *abortPtr, quint16 length);
        virtual ActionToExecute handleTimeout();

        virtual int asynchId();
        virtual Property *property();

    private:
        //BACnet specific
        WritePropertyServiceData *_wData;
        quint8 sendTryOuts;
        //CDM handling
        ExternalObjectsHandler *_responseHandler;

        Property *_concernedProperty;
        int _asynchId;
    };

}


#endif // BACNETWRITEPROPERTYSERVICEHANDLER_H
