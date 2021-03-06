#include "internalrprequesthandler.h"

#include "bacnettagparser.h"
#include "bacnetdeviceobject.h"
#include "bacnetreadpropertyack.h"
#include "internalobjectshandler.h"
#include "bacnetapplicationlayer.h"

using namespace Bacnet;

InternalRPRequestHandler::InternalRPRequestHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination,
                                                   BacnetDeviceObject *device,
                                                   BacnetApplicationLayerHandler *appLayer):
    InternalConfirmedRequestHandler(crData, requester, destination),
    _device(device),
    _appLayer(appLayer),
    _asynchId(-1),
    _error(BacnetServicesNS::ReadProperty),
    _response(0)
{
}

InternalRPRequestHandler::~InternalRPRequestHandler()
{
    delete _response;
    _response = 0;
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

bool InternalRPRequestHandler::finishReading_helper(BacnetObject *readObject, int resultCode, BacnetDataInterfaceShared *value)
{
    _asynchId = 0;
    Q_ASSERT(resultCode <= 0);

    if (!_error.hasError()) {
        if (resultCode < 0) {
            //! \todo translate error to bacnet error
            _error.setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
        } else {
            BacnetDataInterfaceShared val;
            if (0 != value)
                val = *value;
            else
                val = readObject->propertyReadInstantly(_data.propertyId, _data.arrayIndex, &_error);
            if (val.isNull()) {
                if (BacnetErrorNS::ClassNoError == _error.errorClass) {//if was not set by the object itself, this is probably due to memory reasons
                    _error.errorClass = BacnetErrorNS::ClassDevice;
                    _error.errorCode = BacnetErrorNS::CodeOperationalProblem;
                    qDebug("Error while instant device reading");
                }
            } else {
                _response = new BacnetReadPropertyAck(_data, val);
            }
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
    finalizeInstant(_appLayer);
    if (deleteAfter)
        *deleteAfter = true;
}

bool InternalRPRequestHandler::execute()
{
    Q_CHECK_PTR(_device);
    Q_ASSERT(!_error.hasError());

    BacnetObject *object = _device->bacnetObject(objIdToNum(_data.objId));
//    Q_CHECK_PTR(object);
    if (0 == object) {
        _asynchId = 0;
        _error.setError(BacnetErrorNS::ClassObject, BacnetErrorNS::CodeUnknownObject);
        finalizeInstant(_appLayer);
        return true;//am done, delete me
    }

    BacnetDataInterfaceShared data(0);
    int readyness = object->propertyReadTry(_data.propertyId, _data.arrayIndex, data, &_error);
    if (readyness < 0) {
        _asynchId = 0;
        if (!_error.hasError())
            _error.setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
        finalizeInstant(_appLayer);
        return true;//am done, delete me
    } else if (Property::ResultOk == readyness) {
        _asynchId = 0;
        finishReading_helper(object, readyness, &data);
        finalizeInstant(_appLayer);
        return true;
    }
    _asynchId = readyness;
    Q_ASSERT(_appLayer->internalHandler());
    _appLayer->internalHandler()->addAsynchronousHandler(QList<int>()<<_asynchId, this);
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
