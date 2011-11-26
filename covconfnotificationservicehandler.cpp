#include "covconfnotificationservicehandler.h"

#include "covnotificationrequestdata.h"

using namespace Bacnet;

CovConfNotificationServiceHandler::CovConfNotificationServiceHandler(CovNotificationRequestData *data):
    _data(data),
    _sendTryOuts(NumberOfRetransmissions)
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

void CovConfNotificationServiceHandler::handleAck(quint8 *ackPtr, quint16 length, ActionToExecute *action)
{
    //! \what to do? Unsubscribe?
    Q_UNUSED(ackPtr); Q_UNUSED(length); Q_UNUSED(action);
}

void CovConfNotificationServiceHandler::handleError(quint8 *errorPtr, quint16 length, ActionToExecute *action)
{
    //! \what to do? Unsubscribe?
    Q_UNUSED(errorPtr); Q_UNUSED(length); Q_UNUSED(action);
}

void CovConfNotificationServiceHandler::handleAbort(quint8 *abortPtr, quint16 length, ActionToExecute *action)
{
    //! \what to do? Unsubscribe?
    Q_UNUSED(abortPtr); Q_UNUSED(length); Q_UNUSED(action);
}

quint32 CovConfNotificationServiceHandler::handleTimeout(ActionToExecute *action)
{
    Q_CHECK_PTR(action);
    --_sendTryOuts;
    if (0 == _sendTryOuts) {
        *action = ExternalConfirmedServiceHandler::DeleteServiceHandler;
        return 0;
    }

    *action = ExternalConfirmedServiceHandler::ResendService;
    return 1000;
}

void CovConfNotificationServiceHandler::handleReject(quint8 *abortPtr, quint16 length, ExternalConfirmedServiceHandler::ActionToExecute *action)
{
}
