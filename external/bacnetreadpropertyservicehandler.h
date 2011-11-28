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
        ReadPropertyServiceHandler(ReadPropertyServiceData *rpData, ExternalObjectsHandler *respHandler, int asynchId, Property *property);
        virtual ~ReadPropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
        virtual ActionToExecute handleError(Error &error);
        virtual ActionToExecute handleReject(BacnetRejectNS::RejectReason rejectReason);
        virtual ActionToExecute handleAbort();
        virtual ActionToExecute handleTimeout();

        virtual int asynchId();
        virtual Property *property();

    private:
        //BACnet specific
        ReadPropertyServiceData *_rpData;
        //CDM handling
        ExternalObjectsHandler *_responseHandler;

        int _asynchId;
        Property *_property;
    };

}

#endif // BACNETREADPROPERTYSERVICEHANDLER_H
