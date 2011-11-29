#ifndef BACNETREADPROPERTYSERVICEHANDLER_H
#define BACNETREADPROPERTYSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"


class PropertySubject;

namespace Bacnet {

    class ExternalPropertyMapping;
    class ReadPropertyServiceData;

    class ReadPropertyServiceHandler:
            public ExternalConfirmedServiceHandler
    {
    public:
        ReadPropertyServiceHandler(ReadPropertyServiceData *rpData, int asynchId, ExternalPropertyMapping *propertyMapping);
        virtual ~ReadPropertyServiceHandler();

    public://functions overridden from BacnetConfirmedServiceHandler
        virtual qint32 toRaw(quint8 *buffer, quint16 length);
        virtual BacnetServicesNS::BacnetConfirmedServiceChoice serviceChoice();

        virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
        virtual ActionToExecute handleError(Error &error);
        virtual ActionToExecute handleReject(BacnetRejectNS::RejectReason rejectReason);
        virtual ActionToExecute handleAbort();
        virtual ActionToExecute handleTimeout();

    private:
        //BACnet specific
        ReadPropertyServiceData *_rpData;

        int _asynchId;
        ExternalPropertyMapping *_propertyMapping;
    };

}

#endif // BACNETREADPROPERTYSERVICEHANDLER_H
