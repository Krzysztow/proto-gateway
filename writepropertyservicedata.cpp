#include "writepropertyservicedata.h"

#include "bacnettagparser.h"

using namespace Bacnet;

WritePropertyServiceData::WritePropertyServiceData()
{
}

WritePropertyServiceData::WritePropertyServiceData(Bacnet::ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                                                   Bacnet::BacnetDataInterface *writeValue, quint32 arrayIndex):
_objectId(objId)
{
    _propValue.value = writeValue;
    _propValue.arrayIndex = arrayIndex;
    _propValue.propertyId = propertyId;
    //! \todo get this priority value from some settings file.
    _propValue.priority = 16;
}

WritePropertyServiceData::~WritePropertyServiceData()
{
    delete _propValue.value;
}

qint32 WritePropertyServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    Q_CHECK_PTR(startPtr);
    quint8 *actualPtr(startPtr);
    quint16 leftLength(buffLength);
    qint32 ret;

    //! \todo There is some duplication between Request and Ack toRaw() functions. Unify it.
    //encode Object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, _objectId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode objId");
        qDebug("BacnetWriteProperty::toRaw() : propertyencoding problem: objId : %d", ret);
        return -1;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode proeprty identifier
    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _propValue.propertyId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode propId");
        qDebug("BacnetWritePropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
        return -2;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode array index if present - OPTIONAL
    if (Bacnet::ArrayIndexNotPresent != _propValue.arrayIndex) {//present
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _propValue.arrayIndex, true, 2);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "BacnetWritePropertyAck::toRaw()", "Cannot encode arrayIndex");
            qDebug("BacnetWritePropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
            return -3;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    ret = BacnetCoder::openingTagToRaw(actualPtr, leftLength, 3);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode opening tag");
        qDebug("BacnetWriteProperty::toRaw() : cannot encode opening tag: : %d", ret);
        return -4;
    }
    actualPtr += ret;
    leftLength -= ret;

    ret = _propValue.value->toRaw(actualPtr, leftLength);
    if (ret < 0) {
        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode value");
        qDebug("BacnetWriteProperty::toRaw() : value problem: value: %d", ret);
        return -4;
    }
    actualPtr += ret;
    leftLength -= ret;

    ret = BacnetCoder::closingTagToRaw(actualPtr, leftLength, 3);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode opening tag");
        qDebug("BacnetWriteProperty::toRaw() : cannot encode opening tag: : %d", ret);
        return -4;
    }
    actualPtr += ret;
    leftLength -= ret;

    if (_propValue.priority != 16) {
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _propValue.priority, true, 4);
        if (ret < 0) {
            Q_ASSERT_X(false, "BacnetWriteProperty::toRaw()", "Cannot encode priority");
            qDebug("BacnetWriteProperty::toRaw() : value problem: priority: %d", ret);
            return -5;
        }
        actualPtr += ret;
    }

    return actualPtr - startPtr;
}

qint32 WritePropertyServiceData::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    quint8 *actualPtr(serviceData);
    qint16 ret(0);
    bool convOk;

    BacnetTagParser bParser(serviceData, buffLength);

    //get object identifier
    ret = bParser.parseNext();
    _objectId = bParser.toObjectId(&convOk);
    if (ret <= 0 || !bParser.isContextTag(0) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get property identifier
    ret = bParser.parseNext();
    _propValue.propertyId = (BacnetProperty::Identifier)bParser.toUInt(&convOk);
    if (ret <= 0 || !bParser.isContextTag(1) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //we are supposed to parse Abstract type, which type depends on the object type and property Id
    _propValue.value = 0;
    ret = BacnetTagParser::parseStructuredData(bParser, _objectId.objectType, _propValue.propertyId,
                                               _propValue.arrayIndex, 3, &_propValue.value);
    if (ret <= 0) //something worng, check it
        return -1;
    actualPtr += ret;

    ret = bParser.parseNext();
    if (0 == ret) {//no priority passed
        _propValue.priority = 0xff;
    } else if (ret < 0) {
        return -1;
    } else {
        _propValue.priority = bParser.toUInt(&convOk);
        if (!convOk)
            return -1;
    }
    //no more needed to be parsed. The property value is object specific and will be extracted when executed,
    return actualPtr - serviceData;
}
