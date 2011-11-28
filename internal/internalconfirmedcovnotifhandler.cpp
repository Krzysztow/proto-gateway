#include "internalconfirmedcovnotifhandler.h"

#include "error.h"
#include "bacnetapplicationlayer.h"
#include "externalobjectshandler.h"

using namespace Bacnet;

InternalConfirmedCovNotifHandler::InternalConfirmedCovNotifHandler(BacnetConfirmedRequestData *crData,
                                                                   BacnetAddress &requester, BacnetAddress &destination, BacnetApplicationLayerHandler *appLayer):
    InternalConfirmedRequestHandler(crData, requester, destination),
    _appLayer(appLayer),
    _error(BacnetServicesNS::ConfirmedCOVNotification)
{
}

InternalConfirmedCovNotifHandler::~InternalConfirmedCovNotifHandler()
{
}

bool InternalConfirmedCovNotifHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;
}

bool InternalConfirmedCovNotifHandler::isFinished()
{
    return true;
}

void InternalConfirmedCovNotifHandler::finalize(bool *deleteAfter)
{
    if (0 != deleteAfter)
        *deleteAfter = true;
}

bool InternalConfirmedCovNotifHandler::execute()
{
    const int UnconfirmedCOVNotifProcId = 0;
    if ( (UnconfirmedCOVNotifProcId == _data._subscribProcess) ) {
        _error.setError(BacnetErrorNS::ClassServices, BacnetErrorNS::CodeOther);
        qDebug("%s : Got confirmed service with 0 subscriber proc id.", __PRETTY_FUNCTION__);
        return true;
    }

    ExternalObjectsHandler *extHandler = _appLayer->externalHandler();
    Q_CHECK_PTR(extHandler);
    if (0 != extHandler) {
        extHandler->covValueChangeNotification(_data, true, &_error);
        //don't set the response, it's simple ack
    }

    return true;
}

qint32 InternalConfirmedCovNotifHandler::frormRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}

bool InternalConfirmedCovNotifHandler::hasError()
{
    return _error.hasError();
}

Error &InternalConfirmedCovNotifHandler::error()
{
    return _error;
}

BacnetServiceData *InternalConfirmedCovNotifHandler::takeResponseData()
{
    //if we have no error, the response is simple ack, this return 0.
    return 0;
}
