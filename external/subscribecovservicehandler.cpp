#include "subscribecovservicehandler.h"

#include "subscribecovservicedata.h"

using namespace Bacnet;

SubscribeCovServiceHandler::SubscribeCovServiceHandler(SubscribeCOVServiceData *serviceData):
    _serviceData(serviceData),
    _sendTryOut(DefaultTimeoutRetries)
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

quint32 SubscribeCovServiceHandler::handleTimeout(ExternalConfirmedServiceHandler::ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    --_sendTryOut;
    if (0 == _sendTryOut) {
        *action = ExternalConfirmedServiceHandler::DeleteServiceHandler;
        return 0;
    }

    *action = ExternalConfirmedServiceHandler::ResendService;
    return 1000;
}

void SubscribeCovServiceHandler::handleAck(quint8 *ackPtr, quint16 length, ExternalConfirmedServiceHandler::ActionToExecute *action)
{
}

void SubscribeCovServiceHandler::handleError(quint8 *errorPtr, quint16 length, ExternalConfirmedServiceHandler::ActionToExecute *action)
{
}

void SubscribeCovServiceHandler::handleAbort(quint8 *abortPtr, quint16 length, ExternalConfirmedServiceHandler::ActionToExecute *action)
{
}

void SubscribeCovServiceHandler::handleReject(quint8 *abortPtr, quint16 length, ExternalConfirmedServiceHandler::ActionToExecute *action)
{
}

qint32 SubscribeCovServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    return _serviceData->toRaw(buffer, length);
}
