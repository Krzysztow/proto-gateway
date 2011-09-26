#include "bacnetconstructeddata.h"

#include "bacnettagparser.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

//StatusFlags
void DeviceStatus::setDeviceStatus(BACnetDeviceStatus status)
{
    _value = status;
}

DataType::DataType DeviceStatus::typeId()
{
    return DataType::BACnetDeviceStatus;
}

//Segmentation
Segmentation::Segmentation(BACnetSegmentation segOption):
        Enumerated(segOption)
{
}

void Segmentation::setSegmentation(Segmentation::BACnetSegmentation segOption)
{
    _value = segOption;
}

Segmentation::BACnetSegmentation Segmentation::segmentation()
{
    return (BACnetSegmentation)_value;
}

DataType::DataType Segmentation::typeId()
{
    return DataType::BACnetSegmentation;
}



//Unsigned16
bool Unsigned16::setInternal(QVariant &value)
{
    bool ok = UnsignedInteger::setInternal(value);
    if (ok) {
        ok = (_value <= MAX_VALUE);
    }
    return ok;
}

DataType::DataType Unsigned16::typeId()
{
    return DataType::Unsigned16;
}

//ObjectTypesSupported
bool ObjectTypesSupported::setInternal(QVariant &value)
{
    bool ok = BitString::setInternal(value);
    if (ok)
        ok = (_value.size() == REQ_BITS_NUM);
    Q_ASSERT(ok);
    return ok;
}

DataType::DataType ObjectTypesSupported::typeId()
{
    return DataType::BACnetObjectTypesSupported;
}

void ObjectTypesSupported::clearObjectsSupported()
{
    _value.clear();
}

void ObjectTypesSupported::addObjectsSupported(QList<BacnetObjectType::ObjectType> objSupported)
{
    foreach (BacnetObjectType::ObjectType supType, objSupported) {
        _value.setBit(supType);
    }
}

//Services Supported
ServicesSupported::ServicesSupported()
{
#warning "Change it to get value from BacnetServices enumeration!"
    _value.resize(40);
}

void ServicesSupported::clearServices()
{
    _value.clear();
}

void ServicesSupported::setServices(QList<quint8> &_servicesToSet)
{
    foreach (quint8 serviceEnum, _servicesToSet) {
        _value.setBit(serviceEnum);
    }
}


DateTime::DateTime()
{

}

DateTime::~DateTime()
{

}

qint32 DateTime::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    qint32 ret;
    qint32 total(0);
    quint8 *actualPtr(ptrStart);

    ret = _date.toRaw(actualPtr, buffLength);
    if (ret<0)
        return -1;
    actualPtr += ret;
    total += ret;
    buffLength -= ret;

    ret = _time.toRaw(actualPtr, buffLength);
    if (ret<0)
        return -2;
    total += ret;

    return total;
}

qint32 DateTime::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    qint32 ret;
    qint32 total(0);
    quint8 *actualPtr(ptrStart);

    ret = BacnetCoder::openingTagToRaw(actualPtr, buffLength, tagNumber);
    if (ret < 0)
        return -1;
    total += ret;
    actualPtr += ret;
    buffLength -= ret;
    ret = toRaw(actualPtr, buffLength);
    if (ret < 0)
        return -2;
    actualPtr += ret;
    total += ret;
    buffLength -= ret;
    ret = BacnetCoder::closingTagToRaw(actualPtr, buffLength, tagNumber);
    if (ret<0)
        return -3;

    total += ret;
    return total;
}

qint32 DateTime::fromRaw(BacnetTagParser &parser)
{
    qint32 total(0);
    qint32 ret;

    ret = _date.fromRaw(parser);
    if (ret < 0)
        return -1;
    total += ret;
    ret = _time.fromRaw(parser);
    if (ret < 0)
        return -2;
    total += ret;

    return total;
}

qint32 DateTime::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint32 total(0);
    qint32 ret;

    ret = parser.parseNext();
    if (ret <0 || !parser.isOpeningTag(tagNum))
        return -1;
    total += ret;
    ret = fromRaw(parser);
    if (ret<0)
        return -2;
    total += ret;
    ret = parser.parseNext();
    if (ret<0 || !parser.isClosingTag(tagNum))
        return -3;
    total += ret;

    return total;
}

bool DateTime::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, "DateTime::setInternal()", "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant DateTime::toInternal()
{
    Q_ASSERT_X(false, "DateTime::setInternal()", "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType DateTime::typeId()
{
    return DataType::BACnetDateTime;
}

TimeStamp::TimeStamp():
        _choiceValue(0)
{
}

TimeStamp::~TimeStamp()
{
    delete _choiceValue;
}

qint32 TimeStamp::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(_choiceValue);
    if (0 != _choiceValue) {
        return _choiceValue->toRaw(ptrStart, buffLength);
    }

    return -1;
}

qint32 TimeStamp::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(_choiceValue);
    if (0 != _choiceValue) {
        return _choiceValue->toRaw(ptrStart, buffLength, tagNumber);
    }

    return -1;
}

qint32 TimeStamp::fromRaw(BacnetTagParser &parser)
{
    static QList<DataType::DataType> choices(
            QList<DataType::DataType>()
            << DataType::Time << DataType::Unsigned << DataType::BACnetDateTime);

    qint32 ret;
    bool isContext;
    ret = parser.nextTagNumber(&isContext);
    if (ret < 0 || ret >= choices.size())
        return -1;

    Q_ASSERT(0 == _choiceValue);//if this is not zero (for instance TimeStamp is reused), we have a memory leak.
    _choiceValue = BacnetDefaultObject::createDataType(choices.at(ret));
    Q_CHECK_PTR(_choiceValue);
    return _choiceValue->fromRaw(parser);
}

qint32 TimeStamp::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    static QList<DataType::DataType> choices(
            QList<DataType::DataType>()
            << DataType::Time << DataType::Unsigned << DataType::BACnetDateTime);

    qint32 ret;
    bool isContext;
    ret = parser.nextTagNumber(&isContext);
    if (ret < 0 || ret >= choices.size())
        return -1;

    Q_ASSERT(0 == _choiceValue);//if this is not zero (for instance TimeStamp is reused), we have a memory leak.
    _choiceValue = BacnetDefaultObject::createDataType(choices.at(ret));
    Q_CHECK_PTR(_choiceValue);
    return _choiceValue->fromRaw(parser, tagNum);
}

bool TimeStamp::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, "DateTime::setInternal()", "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant TimeStamp::toInternal()
{
    Q_ASSERT_X(false, "DateTime::setInternal()", "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType TimeStamp::typeId()
{
    return DataType::BACnetTimeStamp;
}
