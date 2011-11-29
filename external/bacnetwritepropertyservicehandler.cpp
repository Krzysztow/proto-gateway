#include "bacnetwritepropertyservicehandler.h"

#include "writepropertyservicedata.h"
#include "externalobjectshandler.h"
#include "error.h"

using namespace Bacnet;

BacnetWritePropertyServiceHandler::BacnetWritePropertyServiceHandler(WritePropertyServiceData *wData, int asynchId, ExternalPropertyMapping *propertyMapping):
    _wData(wData),
    _asynchId(asynchId),
    _propertyMapping(propertyMapping)
{
    Q_CHECK_PTR(_wData);
    Q_CHECK_PTR(_propertyMapping);
    Q_CHECK_PTR(_propertyMapping->mappedProperty);
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
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::Timeout);
    return ExternalConfirmedServiceHandler::DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    Q_CHECK_PTR(ackPtr);
    Q_ASSERT(0 == length);
    Q_UNUSED(ackPtr);

    if (length > 0) {
        qWarning("BacnetWritePropertyServiceHandler::handleAck() - ack received, but has some additional data");
        if (_asynchId > 0)
            _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
        return DeleteServiceHandler;//we are done - parent may delete us
    }

    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::ResultOk);
    return DeleteServiceHandler;//we are done - parent may delete us
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleError(Error &error)
{
    //! \todo parse Error message and tell strategy what to do!
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleAbort()
{
    //! \todo parse Abort message and tell strategy what to do!
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute BacnetWritePropertyServiceHandler::handleReject(BacnetRejectNS::RejectReason rejectReason)
{
#warning "ExternalConfirmedServiceHandler::ActionToExecute"
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    return DeleteServiceHandler;
}
