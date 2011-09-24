#include "bacnetreadpropertyservice.h"

#include "bacnettagparser.h"
#include "bacnetdeviceobject.h"
#include "bacnetreadpropertyack.h"
#include "bacneterrorack.h"

ReadPropertyServiceHandler::ReadPropertyServiceHandler(Bacnet::ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                          quint32 arrayIndex):
_response(0)
{
    _rpData.objId = objId;
    _rpData.propertyId = propertyId;
    _rpData.arrayIndex = arrayIndex;
}

ReadPropertyServiceHandler::ReadPropertyServiceHandler():
        _response(0)
{
}

ReadPropertyServiceHandler::~ReadPropertyServiceHandler()
{
    delete _response;
}

qint32 ReadPropertyServiceHandler::fromRaw(quint8 *serviceData, quint8 buffLength)
{
    return _rpData.fromRaw(serviceData, buffLength);
}

//BacnetReject::RejectReason ReadPropertyServiceHandler::checkForError_helper(BacnetTagParser &bParser, qint16 parseRet, quint8 expectedTag, bool convertedOk)
//{
//    if (parseRet < 0) {
//        return BacnetReject::ReasonMissingRequiredParameter;
//    } else if ( !bParser.isContextTag(expectedTag)) {
//        return BacnetReject::ReasonInvalidTag;
//    } else if (!convertedOk) {
//        return BacnetReject::ReasonInvalidParameterDataType;
//    }
//    return BacnetReject::ReasonNotRejected;
//}

qint32 ReadPropertyServiceHandler::execute(BacnetDeviceObject *device)
{
    Q_CHECK_PTR(device);
    Q_ASSERT(BacnetError::ClassNoError == _error.errorClass);
    int readyness = device->ensurePropertyReadyRead(_rpData.propertyId);
    if (readyness < 0) {
        if (!_error.hasError()) //! \todo translate error more correctly
        _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    } else if (Property::ResultOk == readyness) {
        _asynchId = 0;
        finishReading_helper(device);
        return 0;
    }
    _asynchId = readyness;
    return readyness;
}

QList<int> ReadPropertyServiceHandler::asynchIds()
{
    if (0 >= _asynchId)
        return QList<int>() << _asynchId;

    return QList<int>();
}

bool ReadPropertyServiceHandler::isReady()
{
    return (0 == _asynchId);
}

//bool ReadPropertyServiceHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object)
bool ReadPropertyServiceHandler::asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object)
{
    Q_UNUSED(device);
    Q_ASSERT((quint32)asynchId == _asynchId);
    Q_ASSERT(0 == _response);
    if ((quint32)asynchId == _asynchId) {//this is response to our querry
        //! \todo change result boolean to int and make it the same as property::result
        return finishReading_helper(object, result);
    }
    return false;
}

bool ReadPropertyServiceHandler::finishReading_helper(BacnetObject *device, int resultCode)
{
    _asynchId = 0;
    delete _response;//just in case

    Q_ASSERT(resultCode <= 0);

    if (resultCode < 0) {
        //! \todo translate error to bacnet error
        _response = 0;
        _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    } else {
        Bacnet::BacnetDataInterface *value = device->propertyReadInstantly(&_rpData, &_error);
        if (0 == value) {
            if (BacnetError::ClassNoError == _error.errorClass) {//if was not set by the object itself, this is probably due to memory reasons
                _error.errorClass = BacnetError::ClassDevice;
                _error.errorCode = BacnetError::CodeOperationalProblem;
                qDebug("Error while instant device reading");
            }
        } else {
            _response = new BacnetReadPropertyAck(_rpData, value);
        }
    }
    return true;
}

bool ReadPropertyServiceHandler::hasError()
{
    return _error.hasError();
}

Bacnet::Error &ReadPropertyServiceHandler::error()
{
    return _error;
}

BacnetService *ReadPropertyServiceHandler::takeResponse()
{
    Q_ASSERT_X(false, "bacnetreadpropertyservice.cpp", "This handler is obsolete, use InternalRPRequestHandler");
    if (0 != _response) {
        BacnetReadPropertyAck *tmp = _response;
        _response = 0;//we returned ownership as well
//        return tmp;//commented out, since the datatype doesn't match anymore after factorization.
        return 0;
    } else if (BacnetError::CodeNoError != _error.errorCode) {
        return new BacnetErrorAck(_error);
    } else {
        qDebug("BacnetReadPropertyAck::takeResponse() - response not prepared but requested!");
        Q_ASSERT_X(false, "ReadPropertyServiceHandler::takeResponse()", "Response not ready, but requested!");
        return 0;
    }
}

qint32 ReadPropertyServiceHandler::toRaw(quint8 *startPtr, quint8 buffLength)
{
    return _rpData.toRaw(startPtr, buffLength);
}

//#define READ_PRPTY_TEST
#ifdef READ_PRPTY_TEST
int main()
{
    quint8 readPrprtData[] = {
        0x0C,
        0x00, 0x00, 0x00, 0x05,
        0x19,
        0x55
    };

    ReadPropertyServiceHandler readSrvc;
    qint32 ret = readSrvc.fromRaw(readPrprtData, sizeof(readPrprtData));

    qDebug("Read %d bytes!", ret);
    Q_ASSERT(7 == ret);
    Q_ASSERT(BacnetObjectType::AnalogInput == readSrvc._value.objId.objectType);
    Q_ASSERT(5 == readSrvc._value.objId.instanceNum);
    Q_ASSERT(BacnetProperty::PresentValue == readSrvc._value.propertyId);

    return 0;
}

#endif
