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
        virtual void handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action) = 0;
        virtual void handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action) = 0;
        virtual void handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action) = 0;
        virtual void handleReject(quint8 *abortPtr, quint16 length, ActionToExecute *action) = 0;
        virtual quint32 handleTimeout(ActionToExecute *action) = 0;

        virtual int asynchId() = 0;
        virtual Property *property() = 0;
    };

}

#endif // EXTERNALCONFIRMEDSERVICEHANDLER_H
