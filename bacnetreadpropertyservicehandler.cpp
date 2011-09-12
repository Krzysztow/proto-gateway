#include "bacnetreadpropertyservicehandler.h"

#include "bacnetreadpropertyservice.h"
#include "bacnetreadpropertyack.h"
#include "externalobjectshandler.h"

using namespace Bacnet;

ReadPropertyServiceHandlerHandler::ReadPropertyServiceHandlerHandler(ReadPropertyServiceData *rpData, ExternalObjectsHandler *respHandler):
        _rpData(rpData),
        sendTryOuts(3),
        _responseHandler(respHandler)
{
    Q_CHECK_PTR(_rpData);
    Q_CHECK_PTR(_responseHandler);
}

ReadPropertyServiceHandlerHandler::~ReadPropertyServiceHandlerHandler()
{
    delete _rpData;
}

qint32 ReadPropertyServiceHandlerHandler::toRaw(quint8 *buffer, quint16 length)
{
    if (0 == _rpData)
        return -1;
    return _rpData->toRaw(buffer, length);
}

quint32 ReadPropertyServiceHandlerHandler::handleTimeout(ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    --sendTryOuts;
    if (0 == sendTryOuts) {
        *action = BacnetConfirmedServiceHandler::DeleteServiceHandler;
        return 0;
    }

    *action = BacnetConfirmedServiceHandler::ResendService;
    return 1000;
}

void ReadPropertyServiceHandlerHandler::handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    BacnetReadPropertyAck ack;
    qint32 ret = ack.fromRaw(ackPtr, length);
    if (ret < 0) {
        qWarning("ReadPropertyServiceHandlerHandler::handleAck() - ack received, but problem on parsing %d", ret);
        *action = DeleteServiceHandler;//we are done - parent may delete us
    }

    Q_ASSERT(_rpData->objId.instanceNum == ack._value.objId.instanceNum);
    Q_ASSERT(_rpData->objId.objectType == ack._value.objId.objectType);
    Q_ASSERT(_rpData->propertyId == ack._value.propertyId);
    Q_ASSERT(_rpData->arrayIndex == ack._value.arrayIndex);
    delete _rpData; _rpData = 0;

    _responseHandler->handleResponse(this, ack);

    *action = DeleteServiceHandler;//we are done - parent may delete us
}

void ReadPropertyServiceHandlerHandler::handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Error message.
    Error error;
    error.setError(BacnetError::ClassDevice, BacnetError::CodeUnknownObject);
    _responseHandler->handleError(this, error);
    *action = DeleteServiceHandler;
}

void ReadPropertyServiceHandlerHandler::handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Abort message.
    _responseHandler->handleAbort(this, 0);
    *action = DeleteServiceHandler;
}
