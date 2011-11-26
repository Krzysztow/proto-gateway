#include "internalihaveservicehandler.h"
#include "bacnetapplicationlayer.h"
using namespace Bacnet;

InternalIHaveServiceHandler::InternalIHaveServiceHandler(BacnetAddress &requester, BacnetApplicationLayerHandler *appLayer):
    _requester(requester),
    _appLayer(appLayer)
{
}

bool InternalIHaveServiceHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "This function is not meant to be called!");
    return true;
}

bool InternalIHaveServiceHandler::isFinished()
{
    return true;
}

void InternalIHaveServiceHandler::finalize(bool *deleteAfter)
{
    if (0 != deleteAfter)
        *deleteAfter = true;
}

bool InternalIHaveServiceHandler::execute()
{
    Q_ASSERT(_requester.isAddrInitialized());
    Q_CHECK_PTR(_appLayer);
    _appLayer->registerObject(_requester, _data._devId, _data._objId, _data._objName);
    return true;
}

qint32 InternalIHaveServiceHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}

