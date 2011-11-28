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

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleError(quint8 *errorPtr, quint16 length)
{
    //! \what to do? Unsubscribe?
    Q_UNUSED(errorPtr); Q_UNUSED(length);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleAbort(quint8 *abortPtr, quint16 length)
{
    //! \what to do? Unsubscribe?
    Q_UNUSED(abortPtr); Q_UNUSED(length);
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleTimeout()
{
    return DeleteServiceHandler;
}

ExternalConfirmedServiceHandler::ActionToExecute CovConfNotificationServiceHandler::handleReject(quint8 *abortPtr, quint16 length)
{
    return DeleteServiceHandler;
}

int CovConfNotificationServiceHandler::asynchId()
{
    Q_ASSERT(false);
    return 0;
}

Property *CovConfNotificationServiceHandler::property()
{
    Q_ASSERT(false);
    return 0;
}
