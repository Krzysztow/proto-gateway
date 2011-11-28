#ifndef EXTERNALCONFIRMEDSERVICEHANDLER_H
#define EXTERNALCONFIRMEDSERVICEHANDLER_H

#include <QtCore>
#include "bacnetcommon.h"

class Property;

namespace Bacnet {

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
        virtual ActionToExecute handleError(quint8 *errorPtr, quint16 length) = 0;
        virtual ActionToExecute handleAbort(quint8 *abortPtr, quint16 length) = 0;
        virtual ActionToExecute handleReject(quint8 *abortPtr, quint16 length) = 0;
        virtual ActionToExecute handleTimeout() = 0;

        virtual int asynchId() = 0;
        virtual Property *property() = 0;
    };

}

#endif // EXTERNALCONFIRMEDSERVICEHANDLER_H
