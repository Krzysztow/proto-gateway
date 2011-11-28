#include "subscribecovservicehandler.h"

#include "subscribecovservicedata.h"

using namespace Bacnet;

SubscribeCovServiceHandler::SubscribeCovServiceHandler(SubscribeCOVServiceData *serviceData):
    _serviceData(serviceData)
{
}

SubscribeCovServiceHandler::~SubscribeCovServiceHandler()
{
    delete _serviceData;
    _serviceData = 0;
}

int SubscribeCovServiceHandler::asynchId()
{
    return 0;
}

Property *SubscribeCovServiceHandler::property()
{
    return 0;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleTimeout()
{
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    Q_CHECK_PTR(ackPtr);
    Q_ASSERT(0 == length);

    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleError(quint8 *errorPtr, quint16 length)
{
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleAbort(quint8 *abortPtr, quint16 length)
{
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleReject(quint8 *abortPtr, quint16 length)
{
    return DeleteServiceHandler;
}

qint32 SubscribeCovServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    return _serviceData->toRaw(buffer, length);
}
