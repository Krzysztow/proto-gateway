#include "propertyvalue.h"

#include "bacnettagparser.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

PropertyValue::PropertyValue()
{
    _value.value = 0;
}

PropertyValue::PropertyValue(BacnetProperty::Identifier propertyId, BacnetDataInterface *value,
              quint32 arrayIndex, quint8 priority)
{
    _value.propertyId = propertyId;
    _value.value = value;
    Q_CHECK_PTR(value);
    _value.arrayIndex = arrayIndex;
    _value.priority = priority;
}

PropertyValue::~PropertyValue()
{
    delete _value.value;
}

qint32 PropertyValue::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint8 *actualPtr(ptrStart);
    qint32 ret;

    //set property identifier
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _value.propertyId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "PropertyValue::toRaw()", "Cannot encode property id.");
        qDebug("PropertyValue::toRaw() : Cannot encode property id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //set array index, if exists
    if (Bacnet::ArrayIndexNotPresent != _value.arrayIndex) {//is present
        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _value.arrayIndex, true, 1);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "PropertyValue::toRaw()", "Cannot encode array idx.");
            qDebug("PropertyValue::toRaw() : Cannot encode array idx: %d", ret);
            return ret;
        }
        actualPtr += ret;
        buffLength -= ret;
    }

    //write sequence of
    ret = BacnetCoder::openingTagToRaw(actualPtr,  buffLength, 2);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;

    ret = _value.value->toRaw(actualPtr, buffLength);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;

    ret = BacnetCoder::closingTagToRaw(actualPtr, buffLength, 2);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;

    //set priority if necessary
    if (PriorityValueNotPresent != _value.priority) {
        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _value.priority, true, 3);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "PropertyValue::toRaw()", "Cannot encode priority.");
            qDebug("PropertyValue::toRaw() : Cannot encode priority: %d", ret);
            return ret;
        }
        actualPtr += ret;
        buffLength -= ret;
    }

    return (actualPtr - ptrStart);
}

qint32 PropertyValue::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_UNUSED(ptrStart); Q_UNUSED(buffLength); Q_UNUSED(tagNumber);
    Q_ASSERT(false);
    return -1;
}

qint32 PropertyValue::fromRawSpecific(BacnetTagParser &parser, BacnetObjectType::ObjectType objType)
{
    qint32 ret(0);
    qint32 total(0);
    bool convOkOrCtxt;

    //get property identifier
    ret = parser.parseNext();
    _value.propertyId = (BacnetProperty::Identifier)parser.toUInt(&convOkOrCtxt);
    if (ret <= 0 || !parser.isContextTag(0) || !convOkOrCtxt) //not enough data, not context tag or not this tag
        return -1;
    total += ret;

    //get array index, if exists
    ret = parser.nextTagNumber(&convOkOrCtxt);
    if (ret < 0)
        return -1;
    if ( (1 == ret) && convOkOrCtxt) {
        ret = parser.parseNext();
        _value.arrayIndex = parser.toUInt(&convOkOrCtxt);
        if ( (ret < 0) || !convOkOrCtxt )
            return -1;
        total += ret;
    } else {
        _value.arrayIndex = Bacnet::ArrayIndexNotPresent;
    }

    //we are supposed to parse Abstract type, which type depends on the object type and property Id
    _value.value = 0;
    ret = BacnetTagParser::parseStructuredData(parser, objType, _value.propertyId, _value.arrayIndex,
                                         2, &_value.value);
    if (ret < 0 || (0 == _value.value) )
        return -2;

//    if (ret <= 0) //something worng, check it
//        return -1;
    total += ret;

    ret = parser.nextTagNumber(&convOkOrCtxt);
    if ( (3 == ret) && (convOkOrCtxt) ) {
        ret = parser.parseNext();
        if (ret < 0) {
            return -1;
        } else {
            _value.priority = parser.toUInt(&convOkOrCtxt);
            if (!convOkOrCtxt)
                return -1;
            total += ret;
        }
    } else {
        _value.priority = PriorityValueNotPresent;
    }

    return total;
}

qint32 PropertyValue::fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectType::ObjectType objType)
{
    qint32 total(0);
    qint32 ret;

    ret = parser.parseNext();
    if ( (ret < 0) || !parser.isOpeningTag(tagNum) )
        return -1;
    total += ret;
    ret = fromRawSpecific(parser, objType);
    if ( ret < 0 )
        return -2;
    total += ret;
    ret = parser.parseNext();
    if ( (ret < 0) || !parser.isClosingTag(tagNum) )
        return -1;
    total += ret;

    return total;
}

