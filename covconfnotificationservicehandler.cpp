#include "covconfnotificationservicehandler.h"

#include "covnotificationrequestdata.h"

using namespace Bacnet;

CovConfNotificationServiceHandler::CovConfNotificationServiceHandler(CovNotificationRequestData *data):
    _data(data)
{
}

CovConfNotificationServiceHandler::~CovConfNotificationServiceHandler()
{
    delete _data;
}

qint32 CovConfNotificationServiceHandler::toRaw(quint8 *buffer, quint16 length)
{
    Q_CHECK_PTR(_data);
    if (0 != _data)
        return _data->toRaw(buffer, length);
    return -1;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleAck(quint8 *ackPtr, quint16 length)
{
    //! \what to do? Unsubscribe?
    Q_UNUSED(ackPtr); Q_UNUSED(length);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleError(Error &error)
{
    //! \what to do? Unsubscribe?
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleAbort()
{
    //! \what to do? Unsubscribe?
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleTimeout()
{
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleReject(BacnetRejectNS::RejectReason rejectReason)
{
    return DeleteServiceHandler;
}

BacnetServicesNS::BacnetConfirmedServiceChoice CovConfNotificationServiceHandler::serviceChoice()
{
    return BacnetServicesNS::ConfirmedCOVNotification;
}
