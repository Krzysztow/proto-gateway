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
    virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
    virtual ActionToExecute handleError(quint8 *errorPtr, quint16 length);
    virtual ActionToExecute handleReject(quint8 *abortPtr, quint16 length);
    virtual ActionToExecute handleAbort(quint8 *abortPtr, quint16 length);
    virtual ActionToExecute handleTimeout();

    virtual int asynchId();
    virtual Property *property();

public:
    static const quint8 NumberOfRetransmissions = 3;

private:
    CovNotificationRequestData *_data;
};

}

#endif // COVCONFNOTIFICATIONSERVICEHANDLER_H
