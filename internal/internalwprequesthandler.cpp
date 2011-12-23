#include "internalwprequesthandler.h"

#include "bacnetcommon.h"
#include "internalobjectshandler.h"
#include "ihaveservicedata.h"
#include "bacnetdeviceobject.h"
#include "bacnettsm2.h"
#include "bacnetapplicationlayer.h"

using namespace Bacnet;

InternalWPRequestHandler::InternalWPRequestHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination,
                                                   BacnetDeviceObject *device,
                                                   BacnetApplicationLayerHandler *appLayer):
    InternalConfirmedRequestHandler(crData, requester, destination),
    _device(device),
    _appLayer(appLayer),
    _asynchId(-1),
    _error(BacnetServicesNS::WriteProperty)
{
}

InternalWPRequestHandler::~InternalWPRequestHandler()
{
}

bool InternalWPRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(object);
    Q_ASSERT(asynchId == _asynchId);
    if ((quint32)asynchId == _asynchId) {//this is response to our querry
        return finishWriting_helper(device, result);
    }
    return false;
}

bool InternalWPRequestHandler::finishWriting_helper(BacnetDeviceObject *object, int resultCode)
{
    Q_UNUSED(object);
    _asynchId = 0;
    Q_ASSERT(resultCode <= 0);

    if (resultCode < 0) {
        //! \todo translate error to bacnet error
        _error.setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
    } else {
        //nothing to do - response 0 will result in the simple ACK sent.
    }

    return true;
}

bool InternalWPRequestHandler::isFinished()
{
    return (0 == _asynchId);
}

void InternalWPRequestHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    finalizeInstant(_appLayer);
    if (deleteAfter)
        *deleteAfter = true;
}

bool InternalWPRequestHandler::execute()
{
    Q_CHECK_PTR(_device);
    Q_ASSERT(!_error.hasError());

    BacnetObject *object = _device->bacnetObject(_data._objectId.objectIdNum());
    Q_CHECK_PTR(object);
    if (0 == object) {
        _asynchId = 0;
        _error.setError(BacnetErrorNS::ClassObject, BacnetErrorNS::CodeUnknownObject);
        finalizeInstant(_appLayer);
        return true;
    }

    int readyness = object->propertySet(_data._propValue._propertyId, _data._propValue._arrayIndex,
                                        _data._propValue._value, &_error);
    if (readyness < 0) {
        _asynchId = 0;
        if (!_error.hasError())
            _error.setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
        finishWriting_helper(_device, readyness);
        finalizeInstant(_appLayer);
        return true;
    } else if (Property::ResultOk == readyness) {
        _asynchId = 0;
        finishWriting_helper(_device, readyness);
        finalizeInstant(_appLayer);
        return true;
    }
    _asynchId = readyness;
    Q_CHECK_PTR(_appLayer->internalHandler());
    //! \todo Refactor it!
    _appLayer->internalHandler()->addAsynchronousHandler(QList<int>()<<_asynchId, this);
    return false;
}

bool InternalWPRequestHandler::hasError()
{
    return _error.hasError();
}

Error &InternalWPRequestHandler::error()
{
    return _error;
}

Bacnet::BacnetServiceData *InternalWPRequestHandler::takeResponseData()
{
    return 0;
}

qint32 InternalWPRequestHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
