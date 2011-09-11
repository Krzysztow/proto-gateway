#include "bacnetreadpropertyservicehandler.h"

#include "bacnetreadpropertyservice.h"
#include "bacnetreadpropertyack.h"
#include "externalobjectshandler.h"

using namespace Bacnet;

BacnetReadPropertyServiceHandler::BacnetReadPropertyServiceHandler(BacnetReadPropertyService *rpData, ExternalObjectsHandler *respHandler):
        _rpData(rpData),
        sendTryOuts(3),
        _responseHandler(respHandler)
{
    Q_CHECK_PTR(_rpData);
    Q_CHECK_PTR(_responseHandler);
}

BacnetReadPropertyServiceHandler::~BacnetReadPropertyServiceHandler()
{
    delete _rpData;
}

qint32 BacnetReadPropertyServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    if (0 == _rpData)
        return -1;
    return _rpData->toRaw(buffer, length);
}

quint32 BacnetReadPropertyServiceHandler::handleTimeout(ActionToExecute *action)
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

void BacnetReadPropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    BacnetReadPropertyAck ack;
    qint32 ret = ack.fromRaw(ackPtr, length);
    if (ret < 0) {
        qWarning("BacnetReadPropertyServiceHandler::handleAck() - ack received, but problem on parsing %d", ret);
        *action = DeleteServiceHandler;//we are done - parent may delete us
    }

    Q_ASSERT(_rpData->_value.objId.instanceNum == ack._value.objId.instanceNum);
    Q_ASSERT(_rpData->_value.objId.objectType == ack._value.objId.objectType);
    Q_ASSERT(_rpData->_value.propertyId == ack._value.propertyId);
    Q_ASSERT(_rpData->_value.arrayIndex == ack._value.arrayIndex);
    delete _rpData; _rpData = 0;

    _responseHandler->handleResponse(this, ack);

    *action = DeleteServiceHandler;//we are done - parent may delete us
}

void BacnetReadPropertyServiceHandler::handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Error message.
    Error error;
    error.setError(BacnetError::ClassDevice, BacnetError::CodeUnknownObject);
    _responseHandler->handleError(this, error);
    *action = DeleteServiceHandler;
}

void BacnetReadPropertyServiceHandler::handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Abort message.
    _responseHandler->handleAbort(this, 0);
    *action = DeleteServiceHandler;
}
