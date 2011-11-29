#include "subscribecovservicehandler.h"

#include "subscribecovservicedata.h"
#include "externalobjectshandler.h"
#include "error.h"

using namespace Bacnet;

SubscribeCovServiceHandler::SubscribeCovServiceHandler(SubscribeCOVServiceData *serviceData, ExternalObjectsHandler *handler,
                                                       ExternalPropertyMapping *propertyMapping, CovReadStrategy *covStrategy):
    _serviceData(serviceData),
    _handler(handler),
    _propertyMapping(propertyMapping),
    _readStrategy(covStrategy)
{
    Q_CHECK_PTR(_serviceData);
    Q_CHECK_PTR(_handler);
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
    _handler->covSubscriptionProcessFinished(_serviceData->_subscriberProcId, _propertyMapping, _readStrategy, false, false);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    Q_CHECK_PTR(ackPtr);
    Q_ASSERT(0 == length);

    Q_UNUSED(ackPtr);
    Q_UNUSED(length);

    _handler->covSubscriptionProcessFinished(_serviceData->_subscriberProcId, _propertyMapping, _readStrategy, true, false);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleError(Error &error)
{
    bool errorCritical(false);

    switch (error.errorCode) {
    case (BacnetErrorNS::CodeNotCovProperty):
    case (BacnetErrorNS::CodeNoSpaceToAddListElement):
    case (BacnetErrorNS::CodeInconsistentParameters):
    case (BacnetErrorNS::CodeMissingRequiredParameter):
            errorCritical = true;
    default:;
    }

    _handler->covSubscriptionProcessFinished(_serviceData->_subscriberProcId, _propertyMapping, _readStrategy, false, errorCritical);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleAbort()
{
    _handler->covSubscriptionProcessFinished(_serviceData->_subscriberProcId, _propertyMapping, _readStrategy, false, false);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute SubscribeCovServiceHandler::handleReject(BacnetRejectNS::RejectReason rejectReason)
{
    Q_UNUSED(rejectReason);
    //when device aborts the request, that means something really bad happens - it's critical. Most probably our message was ill formed.

    _handler->covSubscriptionProcessFinished(_serviceData->_subscriberProcId, _propertyMapping, _readStrategy, false, true);
    return DeleteServiceHandler;
}

qint32 SubscribeCovServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    return _serviceData->toRaw(buffer, length);
}
