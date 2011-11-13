#include "bacnetwritepropertyservice.h"

#include "bacnettagparser.h"
#include "bacnetdefaultobject.h"
#include "bacnetdeviceobject.h"

BacnetWritePropertyService::BacnetWritePropertyService():
        _asynchId(0)
{
}


BacnetWritePropertyService::BacnetWritePropertyService(Bacnet::ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                                                       Bacnet::BacnetDataInterface *writeValue, quint32 arrayIndex):
    _objectId(objId),
    _propValue(propertyId, writeValue, arrayIndex, 16),
    _asynchId(0)
{
}

BacnetWritePropertyService::~BacnetWritePropertyService()
{
}

qint16 BacnetWritePropertyService::fromRaw(quint8 *servicePtr, quint16 length)
{
    quint8 *actualPtr(servicePtr);
    qint16 ret(0);

    Bacnet::BacnetTagParser bParser(servicePtr, length);

    //get object identifier
    ret = _objectId.fromRaw(bParser, 0);
    if (ret <= 0) //not enough data, not context tag or not this tag
        return ret;
    actualPtr += ret;
    length -= ret;

    //get property value
    ret = _propValue.fromRawSpecific(bParser, _objectId.type(), 1);
    if (ret <= 0)
        return ret;
    actualPtr += ret;
    length -= ret;

//    //get property identifier
//    ret = bParser.parseNext();
//    _propValue._propertyId = (BacnetProperty::Identifier)bParser.toUInt(&convOk);
//    if (ret <= 0 || !bParser.isContextTag(1) || !convOk) //not enough data, not context tag or not this tag
//        return -1;
//    actualPtr += ret;

//    //get arra

//    //we are supposed to parse Abstract type, which type depends on the object type and property Id
//    _propValue._value.clear();
//    ret = Bacnet::BacnetTagParser::parseStructuredData(bParser, _objectId.objectType, _propValue._propertyId,
//                                               _propValue._arrayIndex, 3, _propValue._value);
//    if (ret <= 0) //something worng, check it
//        return -1;
//    actualPtr += ret;

//    ret = bParser.parseNext();
//    if (0 == ret) {//no priority passed
//        _propValue._priority = 0xff;
//    } else if (ret < 0) {
//        return -1;
//    } else {
//        _propValue._priority = bParser.toUInt(&convOk);
//        if (!convOk)
//            return -1;
//    }

    //no more needed to be parsed. The property value is object specific and will be extracted when executed,
    return actualPtr - servicePtr;
}

qint32 BacnetWritePropertyService::toRaw(quint8 *startPtr, quint8 buffLength)
{
    Q_CHECK_PTR(startPtr);
    quint8 *actualPtr(startPtr);
    quint16 leftLength(buffLength);
    qint32 ret;

    //! \todo There is some duplication between Request and Ack toRaw() functions. Unify it.
    //encode Object identifier
    ret = _objectId.toRaw(actualPtr, leftLength, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode objId");
        qDebug("BacnetWriteProperty::toRaw() : propertyencoding problem: objId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    //encode PropertyValue
    ret = _propValue.toRaw(actualPtr, leftLength, 1);
    if (ret < 0)
        return ret;
    actualPtr += ret;
    leftLength -= ret;

//    //encode proeprty identifier
//    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _propValue._propertyId, true, 1);
//    if (ret <= 0) {//something wrong?
//        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode propId");
//        qDebug("BacnetWritePropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
//        return -2;
//    }
//    actualPtr += ret;
//    leftLength -= ret;
//    //encode array index if present - OPTIONAL
//    if (Bacnet::ArrayIndexNotPresent != _propValue._arrayIndex) {//present
//        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _propValue._arrayIndex, true, 2);
//        if (ret <= 0) {//something wrong?
//            Q_ASSERT_X(false, "BacnetWritePropertyAck::toRaw()", "Cannot encode arrayIndex");
//            qDebug("BacnetWritePropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
//            return -3;
//        }
//        actualPtr += ret;
//        leftLength -= ret;
//    }

//    ret = BacnetCoder::openingTagToRaw(actualPtr, leftLength, 3);
//    if (ret <= 0) {
//        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode opening tag");
//        qDebug("BacnetWriteProperty::toRaw() : cannot encode opening tag: : %d", ret);
//        return -4;
//    }
//    actualPtr += ret;
//    leftLength -= ret;

//    ret = _propValue._value->toRaw(actualPtr, leftLength);
//    if (ret < 0) {
//        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode value");
//        qDebug("BacnetWriteProperty::toRaw() : value problem: value: %d", ret);
//        return -4;
//    }
//    actualPtr += ret;
//    leftLength -= ret;

//    ret = BacnetCoder::closingTagToRaw(actualPtr, leftLength, 3);
//    if (ret <= 0) {
//        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode opening tag");
//        qDebug("BacnetWriteProperty::toRaw() : cannot encode opening tag: : %d", ret);
//        return -4;
//    }
//    actualPtr += ret;
//    leftLength -= ret;

//    if (_propValue._priority != 16) {
//        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _propValue._priority, true, 4);
//        if (ret < 0) {
//            Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode priority");
//            qDebug("BacnetWriteProperty::toRaw() : value problem: priority: %d", ret);
//            return -5;
//        }
//        actualPtr += ret;
//    }

    return actualPtr - startPtr;
}


bool BacnetWritePropertyService::isReady()
{
    return (0 == _asynchId);
}

QList<int> BacnetWritePropertyService::asynchIds()
{
    if (0 >= _asynchId)
        return QList<int>() << _asynchId;

    return QList<int>();
}

BacnetService *BacnetWritePropertyService::takeResponse()
{
    return 0;
}

bool BacnetWritePropertyService::hasError()
{
    return _error.hasError();
}

Bacnet::Error &BacnetWritePropertyService::error()
{
    return _error;
}

qint32 BacnetWritePropertyService::execute(BacnetDeviceObject *device)
{
    Q_CHECK_PTR(device);
    Q_ASSERT(BacnetError::ClassNoError == _error.errorClass);

    BacnetObject *object = device->bacnetObject(_objectId.objectIdNum());
    Q_CHECK_PTR(object);
    if (0 == object) {
        _error.setError(BacnetError::ClassObject, BacnetError::CodeUnknownObject);
        return 0;
    }

    int readyness = object->ensurePropertyReadySet(_propValue, &_error);
    if (readyness < 0) {
        if (!_error.hasError())
            _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    } else if (Property::ResultOk == readyness) {
        _asynchId = 0;
        finishWriting_helper(device);
        return 0;
    }
    _asynchId = readyness;
    return readyness;
}

bool BacnetWritePropertyService::finishWriting_helper(BacnetDeviceObject *object, int resultCode)
{
    Q_UNUSED(object);
    _asynchId = 0;
    delete _response;//just in case
    Q_ASSERT(resultCode <= 0);
    _response = 0;

    if (resultCode < 0) {
        //! \todo translate error to bacnet error
        _error.setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    } else {
        //nothing to do - response 0 will result in the simple ACK sent.
    }
    return true;
}

bool BacnetWritePropertyService::asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object)
{
    Q_ASSERT((quint32)asynchId == _asynchId);
    Q_ASSERT(0 == _response);
    if ((quint32)asynchId == _asynchId) {//this is response to our querry
        return finishWriting_helper(device, result);
    }
    return false;
}

//#define WRITE_PROPERTY_TEST
#ifdef WRITE_PROPERTY_TEST
int main()
{
    quint8 wpService[] = {
        0x0c,
        0x00, 0x80, 0x00, 0x01,
        0x19,
        0x55,
        0x3e,
        0x44,
        0x43, 0x34, 0x00, 0x00,
        0x3f
    };

    const quint32 wpSize = sizeof wpService;

    BacnetWritePropertyService service;
    qint16 ret = service.fromRaw(wpService, wpSize);
    Q_ASSERT(ret == wpSize);

    const quint32 writeWpSize(64);
    quint8 writeWpService[writeWpSize];

    ret = service.toRaw(writeWpService, writeWpSize);
    qDebug("Size returned %d, should be %d", ret, wpSize);
    HelperCoder::printArray(wpService, wpSize, "Original \t:");
    HelperCoder::printArray(writeWpService, ret, "Written \t:");
    Q_ASSERT(ret > 0);
    Q_ASSERT(memcmp(wpService, writeWpService, wpSize) == 0);
}

#endif
