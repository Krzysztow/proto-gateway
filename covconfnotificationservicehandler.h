#ifndef COVCONFNOTIFICATIONSERVICEHANDLER_H
#define COVCONFNOTIFICATIONSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"

namespace Bacnet {

class CovNotificationRequestData;

class CovConfNotificationServiceHandler:
        public ExternalConfirmedServiceHandler
{
public:
    CovConfNotificationServiceHandler(CovNotificationRequestData *data = 0);
    virtual ~CovConfNotificationServiceHandler();

public://functions overridden from BacnetConfirmedServiceHandler
    virtual qint32 toRaw(quint8 *buffer, quint16 length);
    virtual void handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action);
    virtual void handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action);
    virtual void handleReject(quint8 *abortPtr, quint16 length, ActionToExecute *action);
    virtual void handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action);
    virtual quint32 handleTimeout(ActionToExecute *action);

    virtual int asynchId();
    virtual Property *property();

public:
    static const quint8 NumberOfRetransmissions = 3;

private:
    CovNotificationRequestData *_data;
    quint8 _sendTryOuts;
};

}

#endif // COVCONFNOTIFICATIONSERVICEHANDLER_H
