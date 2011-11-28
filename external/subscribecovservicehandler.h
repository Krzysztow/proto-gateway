#ifndef BACNET_SUBSCRIBECOVSERVICEHANDLER_H
#define BACNET_SUBSCRIBECOVSERVICEHANDLER_H

#include "externalconfirmedservicehandler.h"

namespace Bacnet {

class SubscribeCOVServiceData;
class ExternalObjectsHandler;
class ExternalPropertyMapping;
class CovReadStrategy;

class SubscribeCovServiceHandler:
        public ExternalConfirmedServiceHandler
{
public:
    SubscribeCovServiceHandler(SubscribeCOVServiceData *serviceData, ExternalObjectsHandler *handler, ExternalPropertyMapping *propertyMapping, CovReadStrategy *covStrategy);
    virtual ~SubscribeCovServiceHandler();

public://overridden ExternalConfirmedServiceHandler methods.
    virtual qint32 toRaw(quint8 *buffer, quint16 length);
    virtual ActionToExecute handleAck(quint8 *ackPtr, quint16 length);
    virtual ActionToExecute handleError(Error &error);
    virtual ActionToExecute handleAbort();
    virtual ActionToExecute handleReject(BacnetRejectNS::RejectReason rejectReason);
    virtual ActionToExecute handleTimeout();

    virtual int asynchId();
    virtual Property *property();

public:
    SubscribeCOVServiceData *_serviceData;
    ExternalObjectsHandler *_handler;
    ExternalPropertyMapping *_propertyMapping;
    CovReadStrategy *_readStrategy;
};

} // namespace Bacnet

#endif // BACNET_SUBSCRIBECOVSERVICEHANDLER_H
