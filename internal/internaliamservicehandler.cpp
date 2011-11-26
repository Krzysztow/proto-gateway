#include "internaliamservicehandler.h"
#include "bacnetapplicationlayer.h"

using namespace Bacnet;

InternalIAmServiceHandler::InternalIAmServiceHandler(BacnetApplicationLayerHandler *appLayer, BacnetAddress requester):
    _appLayer(appLayer),
    _requester(requester)
{
}

bool InternalIAmServiceHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "This function is not meant to be called!");
    return true;
}

bool InternalIAmServiceHandler::isFinished()
{
    return true;
}

void InternalIAmServiceHandler::finalize(bool *deleteAfter)
{
    if (0 != deleteAfter)
        *deleteAfter = true;
}

bool InternalIAmServiceHandler::execute()
{
    Q_ASSERT(_requester.isAddrInitialized());
    Q_CHECK_PTR(_appLayer);
    _appLayer->registerDevice(_requester, _data._devObjId, _data._maxApduLength, _data._segmentationSupported, _data._vendorId);
    return true;
}

qint32 Bacnet::InternalIAmServiceHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
