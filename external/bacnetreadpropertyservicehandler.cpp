#include "bacnetreadpropertyservicehandler.h"

#include "bacnetreadpropertyack.h"
#include "bacnetdata.h"
#include "error.h"
#include "externalpropertymapping.h"
#include "propertysubject.h"
#include "externalobjectreadstrategy.h"

using namespace Bacnet;

ReadPropertyServiceHandler::ReadPropertyServiceHandler(ReadPropertyServiceData *rpData, int asynchId, ExternalPropertyMapping *propertyMapping):
    _rpData(rpData),
    _asynchId(asynchId),
    _propertyMapping(propertyMapping)
{
    Q_CHECK_PTR(_rpData);
    Q_CHECK_PTR(_propertyMapping);
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
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::Timeout);
    if (0 != _propertyMapping->readAccessStrategy)
        _propertyMapping->readAccessStrategy->actionFinished(ExternalObjectReadStrategy::FinishedWithError);
    return ExternalConfirmedServiceHandler::DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    BacnetReadPropertyAck ack;
    qint32 ret = ack.fromRaw(ackPtr, length);
    if (ret < 0) {
        qWarning("ReadPropertyServiceHandler::handleAck() - ack received, but problem on parsing %d", ret);
        if (_asynchId > 0)
            _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    } else {
        Q_ASSERT(_rpData->objId.instanceNum == ack._readData.objId.instanceNum);
        Q_ASSERT(_rpData->objId.objectType == ack._readData.objId.objectType);
        Q_ASSERT(_rpData->propertyId == ack._readData.propertyId);
        Q_ASSERT(_rpData->arrayIndex == ack._readData.arrayIndex);
        delete _rpData; _rpData = 0;

        Q_CHECK_PTR(_propertyMapping->mappedProperty);

        BacnetDataInterfaceShared value = ack._data;
        QVariant internalValue = value->toInternal();
        if (_asynchId > 0) {
            _propertyMapping->mappedProperty->setValueSilent(internalValue);
            _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::ResultOk);
        } else
            _propertyMapping->mappedProperty->setValue(internalValue);

        if (0 != _propertyMapping->readAccessStrategy)
            _propertyMapping->readAccessStrategy->actionFinished(ExternalObjectReadStrategy::FinishedOk);
    }

    return DeleteServiceHandler;//we are done - parent may delete us
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleError(Error &error)
{
    Q_UNUSED(error);
    //! \todo parse Error message.
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    if (0 != _propertyMapping->readAccessStrategy)
        _propertyMapping->readAccessStrategy->actionFinished(ExternalObjectReadStrategy::FinishedWithError);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleAbort()
{
    //! \todo parse Abort message.
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    if (0 != _propertyMapping->readAccessStrategy)
        _propertyMapping->readAccessStrategy->actionFinished(ExternalObjectReadStrategy::FinishedCritical);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute ReadPropertyServiceHandler::handleReject(BacnetRejectNS::RejectReason rejectReason)
{
    Q_UNUSED(rejectReason);
    if (_asynchId > 0)
        _propertyMapping->mappedProperty->asynchActionFinished(_asynchId, Property::UnknownError);
    if (0 != _propertyMapping->readAccessStrategy)
        _propertyMapping->readAccessStrategy->actionFinished(ExternalObjectReadStrategy::FinishedWithError);
    return DeleteServiceHandler;
}

BacnetServicesNS::BacnetConfirmedServiceChoice ReadPropertyServiceHandler::serviceChoice()
{
    return BacnetServicesNS::ReadProperty;
}
