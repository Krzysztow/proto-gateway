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
    virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
    virtual ActionToExecute handleError(quint8 *errorPtr, quint16 length);
    virtual ActionToExecute handleAbort(quint8 *abortPtr, quint16 length);
    virtual ActionToExecute handleReject(quint8 *abortPtr, quint16 length);
    virtual ActionToExecute handleTimeout();

    virtual int asynchId();
    virtual Property *property();

private:
    SubscribeCOVServiceData *_serviceData;
};

} // namespace Bacnet

#endif // BACNET_SUBSCRIBECOVSERVICEHANDLER_H
