#include "propertyvalue.h"

#include "bacnettagparser.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

PropertyValue::PropertyValue():
    _value(0)
{
}

PropertyValue::PropertyValue(BacnetProperty::Identifier propertyId, BacnetDataInterface *value,
                             quint32 arrayIndex, quint8 priority):
    _propertyId(propertyId),
    _arrayIndex(arrayIndex),
    _value(value),
    _priority(priority)
{
    Q_CHECK_PTR(_value);
}

PropertyValue::~PropertyValue()
{
    delete _value;
}

qint32 PropertyValue::toRaw(quint8 *ptrStart, quint16 buffLength, int sequenceShift)
{
    quint8 *actualPtr(ptrStart);
    qint32 ret;

    //set property identifier
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _propertyId, true, 0 + sequenceShift);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "PropertyValue::toRaw()", "Cannot encode property id.");
        qDebug("PropertyValue::toRaw() : Cannot encode property id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //set array index, if exists
    if (Bacnet::ArrayIndexNotPresent != _arrayIndex) {//is present
        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _arrayIndex, true, 1 + sequenceShift);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "PropertyValue::toRaw()", "Cannot encode array idx.");
            qDebug("PropertyValue::toRaw() : Cannot encode array idx: %d", ret);
            return ret;
        }
        actualPtr += ret;
        buffLength -= ret;
    }

    //write sequence of
    ret = BacnetCoder::openingTagToRaw(actualPtr,  buffLength, 2  + sequenceShift);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;

    ret = _value->toRaw(actualPtr, buffLength);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;

    ret = BacnetCoder::closingTagToRaw(actualPtr, buffLength, 2 + sequenceShift);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;

    //set priority if necessary
    if (PriorityValueNotPresent != _priority) {
        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _priority, true, 3 + sequenceShift);
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

qint32 PropertyValue::fromRawSpecific(BacnetTagParser &parser, BacnetObjectType::ObjectType objType, int sequenceShift)
{
    qint32 ret(0);
    qint32 total(0);
    bool convOkOrCtxt;

    //get property identifier
    ret = parser.parseNext();
    _propertyId = (BacnetProperty::Identifier)parser.toUInt(&convOkOrCtxt);
    if (ret <= 0 || !parser.isContextTag(0  + sequenceShift) || !convOkOrCtxt) //not enough data, not context tag or not this tag
        return -1;
    total += ret;

    //get array index, if exists
    ret = parser.nextTagNumber(&convOkOrCtxt);
    if (ret < 0)
        return -1;
    if ( ((1 + sequenceShift) == ret) && convOkOrCtxt) {
        ret = parser.parseNext();
        _arrayIndex = parser.toUInt(&convOkOrCtxt);
        if ( (ret < 0) || !convOkOrCtxt )
            return -1;
        total += ret;
    } else {
        _arrayIndex = Bacnet::ArrayIndexNotPresent;
    }

    //we are supposed to parse Abstract type, which type depends on the object type and property Id
    _value = 0;
    ret = BacnetTagParser::parseStructuredData(parser, objType, _propertyId, _arrayIndex,
                                         2  + sequenceShift, &_value);
    if (ret < 0 || (0 == _value) )
        return -2;

//    if (ret <= 0) //something worng, check it
//        return -1;
    total += ret;

    ret = parser.nextTagNumber(&convOkOrCtxt);
    if ( ((3 + sequenceShift) == ret) && (convOkOrCtxt) ) {
        ret = parser.parseNext();
        if (ret < 0) {
            return -1;
        } else {
            _priority = parser.toUInt(&convOkOrCtxt);
            if (!convOkOrCtxt)
                return -1;
            total += ret;
        }
    } else {
        _priority = PriorityValueNotPresent;
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

