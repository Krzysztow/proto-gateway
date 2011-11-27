#ifndef BACNET_SUBSCRIBECOVSERVICEHANDLER_H
#define BACNET_SUBSCRIBECOVSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"

namespace Bacnet {

class SubscribeCOVServiceData;

class SubscribeCovServiceHandler:
        public ExternalConfirmedServiceHandler
{
public:
    SubscribeCovServiceHandler(SubscribeCOVServiceData *serviceData);
    virtual ~SubscribeCovServiceHandler();

public://overridden ExternalConfirmedServiceHandler methods.
    virtual qint32 toRaw(quint8 *buffer, quint16 length);
    virtual void handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action);
    virtual void handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action);
    virtual void handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action);
    virtual void handleReject(quint8 *abortPtr, quint16 length, ActionToExecute *action);
    virtual quint32 handleTimeout(ActionToExecute *action);

    virtual int asynchId();
    virtual Property *property();

private:
    SubscribeCOVServiceData *_serviceData;

    static const int DefaultTimeoutRetries = 3;
    int _sendTryOut;

};

} // namespace Bacnet

#endif // BACNET_SUBSCRIBECOVSERVICEHANDLER_H
