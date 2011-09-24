#include "bacnetreadpropertyservicehandler.h"

#include "bacnetreadpropertyack.h"
#include "externalobjectshandler.h"
#include "bacnetdata.h"
#include "error.h"

using namespace Bacnet;

ReadPropertyServiceHandler::ReadPropertyServiceHandler(ReadPropertyServiceData *rpData, ExternalObjectsHandler *respHandler):
        _rpData(rpData),
        sendTryOuts(3),
        _responseHandler(respHandler)
{
    Q_CHECK_PTR(_rpData);
    Q_CHECK_PTR(_responseHandler);
}

ReadPropertyServiceHandler::~ReadPropertyServiceHandler()
{
    delete _rpData;
}

qint32 ReadPropertyServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    if (0 == _rpData)
        return -1;
    return _rpData->toRaw(buffer, length);
}

quint32 ReadPropertyServiceHandler::handleTimeout(ActionToExecute *action)
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

void ReadPropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    BacnetReadPropertyAck ack;
    qint32 ret = ack.fromRaw(ackPtr, length);
    if (ret < 0) {
        qWarning("ReadPropertyServiceHandler::handleAck() - ack received, but problem on parsing %d", ret);
        *action = DeleteServiceHandler;//we are done - parent may delete us
    }

    Q_ASSERT(_rpData->objId.instanceNum == ack._readData.objId.instanceNum);
    Q_ASSERT(_rpData->objId.objectType == ack._readData.objId.objectType);
    Q_ASSERT(_rpData->propertyId == ack._readData.propertyId);
    Q_ASSERT(_rpData->arrayIndex == ack._readData.arrayIndex);
    delete _rpData; _rpData = 0;

    _responseHandler->handleResponse(this, ack);

    *action = DeleteServiceHandler;//we are done - parent may delete us
}

void ReadPropertyServiceHandler::handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Error message.
    Error error;
    error.setError(BacnetError::ClassDevice, BacnetError::CodeUnknownObject);
    _responseHandler->handleError(this, error);
    *action = DeleteServiceHandler;
}

void ReadPropertyServiceHandler::handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action)
{
    //! \todo parse Abort message.
    _responseHandler->handleAbort(this, 0);
    *action = DeleteServiceHandler;
}
