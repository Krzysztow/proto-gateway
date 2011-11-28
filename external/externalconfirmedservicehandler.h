#ifndef EXTERNALCONFIRMEDSERVICEHANDLER_H
#define EXTERNALCONFIRMEDSERVICEHANDLER_H

#include <QtCore>
#include "bacnetcommon.h"

class Property;

namespace Bacnet {

class Error;

    class ExternalConfirmedServiceHandler
    {
    public:
        enum ActionToExecute {
            DeleteServiceHandler,
            DoNothing,
            ResendService
        };

        virtual ~ExternalConfirmedServiceHandler() {}

        virtual qint32 toRaw(quint8 *buffer, quint16 length) = 0;
        virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length) = 0;
        virtual ActionToExecute handleError(Error &error) = 0;
        virtual ActionToExecute handleAbort() = 0;
        virtual ActionToExecute handleReject(BacnetRejectNS::RejectReason rejectReason) = 0;
        virtual ActionToExecute handleTimeout() = 0;

        virtual int asynchId() = 0;
        virtual Property *property() = 0;
    };

}

#endif // EXTERNALCONFIRMEDSERVICEHANDLER_H
