#include "internalsubscribecovservicehandler.h"

using namespace Bacnet;

InternalSubscribeCovServiceHanlder::InternalSubscribeCovServiceHanlder(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                                       InternalObjectsHandler *internalHandler, ExternalObjectsHandler *externalHandler):
    _tsm(tsm),
    _device(device),
    _internalHandler(internalHandler),
    _externalHandler(externalHandler)

{
}

InternalSubscribeCovServiceHanlder::~InternalSubscribeCovServiceHanlder()
{
}

bool InternalSubscribeCovServiceHanlder::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(object);
    Q_ASSERT((quint32)asynchId == _asynchId);
    if ((quint32)asynchId == _asynchId) {//this is response to our querry
        return finishWriting_helper(device, result);
    }
    return false;
}

bool InternalSubscribeCovServiceHanlder::isFinished()
{
    return true;
}

void InternalSubscribeCovServiceHanlder::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    finalizeInstant(_tsm);
    if (deleteAfter)
        *deleteAfter = true;
}

bool InternalSubscribeCovServiceHanlder::execute()
{
    Q_CHECK_PTR(_device);
    Q_ASSERT(!_error.hasError());

    BacnetObject *object = _device->bacnetObject(objIdToNum(_data.));
    Q_CHECK_PTR(object);
    if (0 == object) {
        _error.setError(BacnetError::ClassObject, BacnetError::CodeUnknownObject);
        finalizeInstant(_tsm);
        return true;
    }


    return true;
}

bool InternalSubscribeCovServiceHanlder::hasError()
{
    return _error.hasError();
}

Error &InternalSubscribeCovServiceHanlder::error()
{
    return _error;
}

Bacnet::BacnetServiceData *InternalSubscribeCovServiceHanlder::takeResponseData()
{
    return 0;
}

qint32 InternalSubscribeCovServiceHanlder::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
