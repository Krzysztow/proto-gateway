#include "bacnetreadpropertyservice.h"

#include "bacnettagparser.h"
#include "bacnetdeviceobject.h"
#include "bacnetreadpropertyack.h"
#include "bacneterrorack.h"

BacnetReadPropertyService::BacnetReadPropertyService(Bacnet::ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                          quint32 arrayIndex):
_response(0)
{
    _value.objId = objId;
    _value.propertyId = propertyId;
    _value.arrayIndex = arrayIndex;
}

BacnetReadPropertyService::BacnetReadPropertyService():
        _response(0)
{
}

BacnetReadPropertyService::~BacnetReadPropertyService()
{
    delete _response;
}

qint32 BacnetReadPropertyService::fromRaw(quint8 *serviceData, quint8 buffLength)
{
    //clear error codes. since we handle new request
    _error.setError(BacnetError::ClassNoError, BacnetError::CodeNoError);

    BacnetTagParser bParser(serviceData, buffLength);

    qint16 ret;
    qint16 consumedBytes(0);
    BacnetReject::RejectReason reason;
    bool convOk;

    //parse object identifier
    ret = bParser.parseNext();
    _value.objId = bParser.toObjectId(&convOk);
    if (ret < 0 || !bParser.isContextTag(0))
        return -1;
//    if ( (reason = checkForError_helper(bParser, ret, 0, convOk)) != BacnetReject::ReasonNotRejected) {
//        return reason;
//    }
    consumedBytes += ret;

    //parse property identifier
    ret = bParser.parseNext();
    _value.propertyId = (BacnetProperty::Identifier)bParser.toUInt(&convOk);
//    if ( (reason = checkForError_helper(bParser, ret, 1, convOk)) != BacnetReject::ReasonNotRejected) {
//        return reason;
//    }
    if (ret < 0 || !bParser.isContextTag(1))
        return -2;
    consumedBytes += ret;

    //parse OPTIONAL array index
    ret = bParser.parseNext();
    if (0 != ret) {//there is something - index or error
        _value.arrayIndex = bParser.toUInt(&convOk);
//        if ( (reason = checkForError_helper(bParser, ret, 2, convOk)) != BacnetReject::ReasonNotRejected) {
//            return reason;
//        }
        if (ret <0 || !bParser.isContextTag(2))
            return -3;
        consumedBytes += ret;
    } else {
        _value.arrayIndex = Bacnet::ArrayIndexNotPresent;
    }

    Q_ASSERT(consumedBytes == buffLength);
    if (consumedBytes != buffLength) {
        return BacnetReject::ReasonTooManyArguments;
    }

    return consumedBytes;
}

//BacnetReject::RejectReason BacnetReadPropertyService::checkForError_helper(BacnetTagParser &bParser, qint16 parseRet, quint8 expectedTag, bool convertedOk)
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

qint32 BacnetReadPropertyService::execute(BacnetDeviceObject *device)
{
    Q_CHECK_PTR(device);
    Q_ASSERT(BacnetError::ClassNoError == _error.errorClass);
    int readyness = device->ensurePropertyReadyRead(_value.propertyId);
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

QList<int> BacnetReadPropertyService::asynchIds()
{
    if (0 >= _asynchId)
        return QList<int>() << _asynchId;

    return QList<int>();
}

bool BacnetReadPropertyService::isReady()
{
    return (0 == _asynchId);
}

//bool BacnetReadPropertyService::asynchActionFinished(int asynchId, int result, BacnetObject *object)
bool BacnetReadPropertyService::asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object)
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

bool BacnetReadPropertyService::finishReading_helper(BacnetObject *device, int resultCode)
{
    _asynchId = 0;
    delete _response;//just in case

    Q_ASSERT(resultCode <= 0);

    if (resultCode < 0) {
        //! \todo translate error to bacnet error
        _response = 0;
        _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    } else {
        Bacnet::BacnetDataInterface *data = device->propertyReadInstantly(&_value, &_error);
        if (0 == data) {
            if (BacnetError::ClassNoError == _error.errorClass) {//if was not set by the object itself, this is probably due to memory reasons
                _error.errorClass = BacnetError::ClassDevice;
                _error.errorCode = BacnetError::CodeOperationalProblem;
                qDebug("Error while instant device reading");
            }
        } else {
            _response = new BacnetReadPropertyAck();
            _response->setData(data, _value);
        }
    }
    return true;
}

bool BacnetReadPropertyService::hasError()
{
    return _error.hasError();
}

Bacnet::Error &BacnetReadPropertyService::error()
{
    return _error;
}

BacnetService *BacnetReadPropertyService::takeResponse()
{
    if (0 != _response) {
        BacnetReadPropertyAck *tmp = _response;
        _response = 0;//we returned ownership as well
        return tmp;
    } else if (BacnetError::CodeNoError != _error.errorCode) {
        return new BacnetErrorAck(_error);
    } else {
        qDebug("BacnetReadPropertyAck::takeResponse() - response not prepared but requested!");
        Q_ASSERT_X(false, "BacnetReadPropertyService::takeResponse()", "Response not ready, but requested!");
        return 0;
    }
}

qint32 BacnetReadPropertyService::toRaw(quint8 *startPtr, quint8 buffLength)
{
    Q_CHECK_PTR(startPtr);
    quint8 *actualPtr(startPtr);
    quint16 leftLength(buffLength);
    qint32 ret;

    //! \todo There is some duplication between Request and Ack toRaw() functions. Unify it.
    //encode Object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, _value.objId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadProperty::toRaw()", "Cannot encode objId");
        qDebug("BacnetReadProperty::toRaw() : propertyencoding problem: objId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode proeprty identifier
    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.propertyId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadProperty::toRaw()", "Cannot encode propId");
        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode array index if present - OPTIONAL
    if (Bacnet::ArrayIndexNotPresent != _value.arrayIndex) {//present
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.arrayIndex, true, 2);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode arrayIndex");
            qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    return actualPtr - startPtr;
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

    BacnetReadPropertyService readSrvc;
    qint32 ret = readSrvc.fromRaw(readPrprtData, sizeof(readPrprtData));

    qDebug("Read %d bytes!", ret);
    Q_ASSERT(7 == ret);
    Q_ASSERT(BacnetObjectType::AnalogInput == readSrvc._value.objId.objectType);
    Q_ASSERT(5 == readSrvc._value.objId.instanceNum);
    Q_ASSERT(BacnetProperty::PresentValue == readSrvc._value.propertyId);

    return 0;
}

#endif
