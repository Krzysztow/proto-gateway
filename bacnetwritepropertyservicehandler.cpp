#include "bacnetwritepropertyservicehandler.h"

#include "bacnetwritepropertyservice.h"
#include "externalobjectshandler.h"

using namespace Bacnet;

BacnetWritePropertyServiceHandler::BacnetWritePropertyServiceHandler(BacnetWritePropertyService *wData, ExternalObjectsHandler *respHandler):
        _wData(wData),
        sendTryOuts(3),
        _responseHandler(respHandler)
{
    Q_CHECK_PTR(_wData);
    Q_CHECK_PTR(_responseHandler);
}

BacnetWritePropertyServiceHandler::~BacnetWritePropertyServiceHandler()
{
    delete _wData;
}

qint32 BacnetWritePropertyServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    if (0 == _wData)
        return -1;
    return _wData->toRaw(buffer, length);
}

quint32 BacnetWritePropertyServiceHandler::handleTimeout(ActionToExecute *action)
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

void BacnetWritePropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    Q_ASSERT(0 == length);

    Q_UNUSED(ackPtr);

    if (length > 0) {
        qWarning("BacnetWritePropertyServiceHandler::handleAck() - ack received, but has some additional data");
        *action = DeleteServiceHandler;//we are done - parent may delete us
    }

    _responseHandler->handleResponse(this, true);
    *action = DeleteServiceHandler;//we are done - parent may delete us
}

void BacnetWritePropertyServiceHandler::handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Error message.
    Error error;
    error.setError(BacnetError::ClassDevice, BacnetError::CodeUnknownObject);
    _responseHandler->handleError(this, error);
    *action = DeleteServiceHandler;
}

void BacnetWritePropertyServiceHandler::handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Abort message.
    _responseHandler->handleAbort(this, 0);
    *action = DeleteServiceHandler;
}
