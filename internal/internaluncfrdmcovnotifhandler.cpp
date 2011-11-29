#include "internaluncfrdmcovnotifhandler.h"

#include "bacnetapplicationlayer.h"
#include "externalobjectshandler.h"

using namespace Bacnet;

InternalUncfrdmCovNotifHandler::InternalUncfrdmCovNotifHandler(BacnetApplicationLayerHandler *appLayer):
    _appLayer(appLayer)
{
    Q_CHECK_PTR(_appLayer);
}

bool Bacnet::InternalUncfrdmCovNotifHandler::asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;
}

bool Bacnet::InternalUncfrdmCovNotifHandler::isFinished()
{
    return true;
}

void Bacnet::InternalUncfrdmCovNotifHandler::finalize(bool *deleteAfter)
{
    if (0 != deleteAfter)
        *deleteAfter = true;
}

bool Bacnet::InternalUncfrdmCovNotifHandler::execute()
{
    ExternalObjectsHandler *extHandler = _appLayer->externalHandler();
    Q_CHECK_PTR(extHandler);
    if (0 != extHandler) {
        extHandler->covValueChangeNotification(_data, false, 0);
        //don't set the response, it's simple ack
    }

    return true;
}

qint32 Bacnet::InternalUncfrdmCovNotifHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
