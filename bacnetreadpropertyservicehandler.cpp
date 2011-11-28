#include "bacnetreadpropertyservicehandler.h"

#include "bacnetreadpropertyack.h"
#include "externalobjectshandler.h"
#include "bacnetdata.h"
#include "error.h"

using namespace Bacnet;

ReadPropertyServiceHandler::ReadPropertyServiceHandler(ReadPropertyServiceData *rpData, ExternalObjectsHandler *respHandler, int asynchId, Property *property):
    _rpData(rpData),
    _responseHandler(respHandler),
    _asynchId(asynchId),
    _property(property)
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

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleTimeout()
{
    return ExternalConfirmedServiceHandler::DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    BacnetReadPropertyAck ack;
    qint32 ret = ack.fromRaw(ackPtr, length);
    if (ret < 0) {
        qWarning("ReadPropertyServiceHandler::handleAck() - ack received, but problem on parsing %d", ret);
        return DeleteServiceHandler;//we are done - parent may delete us
    }

    Q_ASSERT(_rpData->objId.instanceNum == ack._readData.objId.instanceNum);
    Q_ASSERT(_rpData->objId.objectType == ack._readData.objId.objectType);
    Q_ASSERT(_rpData->propertyId == ack._readData.propertyId);
    Q_ASSERT(_rpData->arrayIndex == ack._readData.arrayIndex);
    delete _rpData; _rpData = 0;

    _responseHandler->handleResponse(this, ack);

    return DeleteServiceHandler;//we are done - parent may delete us
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleError(quint8 *errorPtr, quint16 length)
{
    //! \todo parse Error message.
    Error error;
    error.setError(BacnetErrorNS::ClassDevice, BacnetErrorNS::CodeUnknownObject);
    _responseHandler->handleError(this, error);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleAbort(quint8 *abortPtr, quint16 length)
{
    //! \todo parse Abort message.
    _responseHandler->handleAbort(this, 0);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleReject(quint8 *abortPtr, quint16 length)
{
    return DeleteServiceHandler;
}

int ReadPropertyServiceHandler::asynchId()
{
    return _asynchId;
}

Property *ReadPropertyServiceHandler::property()
{
    return _property;
}
