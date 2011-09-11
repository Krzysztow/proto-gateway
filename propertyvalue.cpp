#include "propertyvalue.h"

#include "bacnettagparser.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

PropertyValue::PropertyValue()
{
    _value.value = 0;
}

PropertyValue::~PropertyValue()
{
    delete _value.value;
}

qint32 PropertyValue::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_UNUSED(ptrStart); Q_UNUSED(buffLength);
    Q_ASSERT(false);
    return -1;
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
        _value.priority = 0xff;
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

