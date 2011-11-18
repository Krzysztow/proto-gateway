#include "internalrprequesthandler.h"

#include "bacnettagparser.h"
#include "bacnetdeviceobject.h"
#include "bacnetreadpropertyack.h"
#include "internalobjectshandler.h"

using namespace Bacnet;

InternalRPRequestHandler::InternalRPRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                   InternalObjectsHandler *internalHandler, ExternalObjectsHandler *externalHandler):
InternalConfirmedRequestHandler(/*tsm, device, internalHandler, externalHandler*/),
_tsm(tsm),
_device(device),
_internalHandler(internalHandler),
_externalHandler(externalHandler),
_asynchId(-1)
{
}

InternalRPRequestHandler::~InternalRPRequestHandler()
{
}

bool InternalRPRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(device);
    Q_ASSERT((quint32)asynchId == _asynchId);
    if ((quint32)asynchId == _asynchId) {//this is response to our querry
        return finishReading_helper(object, result);
    }
    return false;
}

bool InternalRPRequestHandler::finishReading_helper(BacnetObject *readObject, int resultCode)
{
    _asynchId = 0;
    Q_ASSERT(resultCode <= 0);

    if (resultCode < 0) {
        //! \todo translate error to bacnet error
        _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    } else {
        Bacnet::BacnetDataInterface *value = readObject->propertyReadInstantly(&_data, &_error);
        if (0 == value) {
            if (BacnetError::ClassNoError == _error.errorClass) {//if was not set by the object itself, this is probably due to memory reasons
                _error.errorClass = BacnetError::ClassDevice;
                _error.errorCode = BacnetError::CodeOperationalProblem;
                qDebug("Error while instant device reading");
            }
        } else {
            _response = new BacnetReadPropertyAck(_data, value);
        }
    }

    return true;
}

BacnetServiceData *InternalRPRequestHandler::takeResponseData()
{
    BacnetServiceData *tmp = _response;
    _response = 0;//we set to zero, since caller takes ownership over the response.
    return tmp;
}

bool InternalRPRequestHandler::isFinished()
{
    return (0 == _asynchId);
}

void InternalRPRequestHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    finalizeInstant(_tsm);
    if (deleteAfter)
        *deleteAfter = true;
}

bool InternalRPRequestHandler::execute()
{
    Q_CHECK_PTR(_device);
    Q_ASSERT(!_error.hasError());

    BacnetObject *object = _device->bacnetObject(objIdToNum(_data.objId));
    Q_CHECK_PTR(object);
    if (0 == object) {
        _error.setError(BacnetError::ClassObject, BacnetError::CodeUnknownObject);
        finalizeInstant(_tsm);
        return true;//am done, delete me
    }

    int readyness = object->isPropertyReadready(_data.propertyId);
    if (readyness < 0) {
        if (!_error.hasError())
            _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
        finalizeInstant(_tsm);
        return true;//am done, delete me
    } else if (Property::ResultOk == readyness) {
        _asynchId = 0;
        finishReading_helper(object, readyness);
        finalizeInstant(_tsm);
        return true;
    }
    _asynchId = readyness;
    _internalHandler->addAsynchronousHandler(QList<int>()<<_asynchId, this);
    return false;//not done, yet - don't delete me
}

bool InternalRPRequestHandler::hasError()
{
    return _error.hasError();
}

Error &InternalRPRequestHandler::error()
{
    return _error;
}

qint32 InternalRPRequestHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
