#include "whohasservicehandler.h"

#include "internalobjectshandler.h"
#include "bacnetdeviceobject.h"
#include "ihaveservicedata.h"
#include "bacnetobject.h"
#include "bacnettsm2.h"

using namespace Bacnet;

/**
  DON"T USE THIS CLASS ANYMORE. INSTEAD USE InternalWhoIsRequestHandler.
  */

WhoHasServiceHandler::WhoHasServiceHandler(InternalObjectsHandler *internalHndlr, BacnetTSM2 *tsm, BacnetDeviceObject *device):
        _internalHndlr(internalHndlr),
        _tsm(tsm),
        _device(device)
{
    Q_CHECK_PTR(internalHndlr);
    Q_CHECK_PTR(tsm);
    //if device is 0, this means we have broadcast.
}

qint32 WhoHasServiceHandler::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    return _data.fromRaw(serviceData, buffLength);
}

bool WhoHasServiceHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;//in case it is, tell it's done.
}

bool WhoHasServiceHandler::isFinished()
{
    return true;
}

void WhoHasServiceHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    if (deleteAfter)
        *deleteAfter = true;
}

QList<int> WhoHasServiceHandler::execute()
{
    Q_ASSERT(false);//this class is not ued anymore - use InternalWhoHasServiceHandler instead.
}
