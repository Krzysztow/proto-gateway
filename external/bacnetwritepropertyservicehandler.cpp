#include "bacnetwritepropertyservicehandler.h"

#include "writepropertyservicedata.h"
#include "externalobjectshandler.h"
#include "error.h"

using namespace Bacnet;

BacnetWritePropertyServiceHandler::BacnetWritePropertyServiceHandler(WritePropertyServiceData *wData, ExternalObjectsHandler *respHandler, Property *property, int asynchId):
    _wData(wData),
    sendTryOuts(3),
    _responseHandler(respHandler),
    _concernedProperty(property),
    _asynchId(asynchId)
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

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleTimeout()
{
#warning "ExternalConfirmedServiceHandler::ActionToExecute"
    return ExternalConfirmedServiceHandler::DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    Q_CHECK_PTR(ackPtr);
    Q_ASSERT(0 == length);
    Q_UNUSED(ackPtr);

    if (length > 0) {
        qWarning("BacnetWritePropertyServiceHandler::handleAck() - ack received, but has some additional data");
        return DeleteServiceHandler;//we are done - parent may delete us
    }

    _responseHandler->handleResponse(this, true);
    return DeleteServiceHandler;//we are done - parent may delete us
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleError(Error &error)
{
    //! \todo parse Error message.
    _responseHandler->handleError(this, error);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleAbort()
{
    //! \todo parse Abort message.
    _responseHandler->handleAbort(this, 0);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleReject(BacnetRejectNS::RejectReason rejectReason)
{
#warning "ExternalConfirmedServiceHandler::ActionToExecute"
    return DeleteServiceHandler;
}

Property *BacnetWritePropertyServiceHandler::property()
{
    return _concernedProperty;
}

int BacnetWritePropertyServiceHandler::asynchId()
{
    return _asynchId;
}
