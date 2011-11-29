#ifndef BACNETWRITEPROPERTYSERVICEHANDLER_H
#define BACNETWRITEPROPERTYSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"

class PropertySubject;

namespace Bacnet {

    class ExternalPropertyMapping;
    class WritePropertyServiceData;

    class BacnetWritePropertyServiceHandler:
            public ExternalConfirmedServiceHandler
    {
    public:
        BacnetWritePropertyServiceHandler(WritePropertyServiceData *wData, int asynchId, ExternalPropertyMapping *propertyMapping);
        virtual ~BacnetWritePropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
        virtual ActionToExecute handleError(Error &error);
        virtual ActionToExecute handleReject(BacnetRejectNS::RejectReason rejectReason);
        virtual ActionToExecute handleAbort();
        virtual ActionToExecute handleTimeout();

    private:
        //BACnet specific
        WritePropertyServiceData *_wData;

        int _asynchId;
        ExternalPropertyMapping *_propertyMapping;
    };

}


#endif // BACNETWRITEPROPERTYSERVICEHANDLER_H
