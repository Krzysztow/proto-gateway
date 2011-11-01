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
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
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
    return fromRawTagEnclosed_helper(parser, tagNum);
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

    return fromRawChoiceValue_helper(parser, choices, _choiceValue);
}

qint32 TimeStamp::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
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


////////////// PropertyReference /////////////
PropertyReference::PropertyReference():
        _identifier(BacnetProperty::UndefinedProperty),
        _arrayIdx(ArrayIndexNotPresent)
{
}

PropertyReference::~PropertyReference(){}

qint32 PropertyReference::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    qint32 ret;
    quint8 *actualPtr(ptrStart);

    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, _identifier, true, 0);
    if (ret<0)
        return -1;
    actualPtr += ret;
    buffLength -= ret;

    if (ArrayIndexNotPresent != _arrayIdx) {
        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _arrayIdx, true, 1);
        if (ret<0)
            return -2;
        actualPtr += ret;
    }

    return actualPtr - ptrStart;
}

qint32 PropertyReference::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 PropertyReference::fromRaw(BacnetTagParser &parser)
{
    bool okOrCtxt;
    //firstly parse obligatory propertyId
    qint16 ret = parser.parseNext();
    if (!parser.isContextTag(0))
        return -1;
    _identifier = parser.toObjectId(&okOrCtxt);
    if (ret < 0 || !okOrCtxt)
        return -1;
    //now check if we are provided with property array index. If so, parse it.
    if (parser.nextTagNumber(&okOrCtxt) == 1 && okOrCtxt) {
        qint16 temp = parser.parseNext();
        if (temp < 0)
            return -2;
        _arrayIdx = parser.toUInt(&okOrCtxt);
        if (!okOrCtxt)
            return -2;
        total += temp;
    } else
        _arrayIdx = ArrayIndexNotPresent;

    return ret;
}

qint32 PropertyReference::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool PropertyReference::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, "PropertyReferecnce::setInternal()", "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant PropertyReference::toInternal()
{
    Q_ASSERT_X(false, "PropertyReferecnce::setInternal()", "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType PropertyReference::typeId()
{
    return DataType::BACnetPropertyRefernce;
}

BacnetProperty::Identifier PropertyReference::identifier()
{
    return _identifier;
}

quint32 PropertyReference::arrayIndex()
{
    return _arrayIdx;
}

////////////////////////////////////////////////////////////////
///////////////ADDRESS//////////////
////////////////////////////////////////////////////////////////

Address::Address()
{
}

Address::Address(BacnetAddress &address):
    _bacAddress(address)
{
}

qint32 Address::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint8 *actualPtr(ptrStart);
    qint32 ret(0), total(0);

    Q_ASSERT(_bacAddress.isAddrInitialized());

    //encode network number
    quint32 netNum = _bacAddress.networkNumber();
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, netNum, true, AppTags::UnsignedInteger);
    if (ret < 0)
        return -1;
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //encode the MAC address part
    Bacnet::OctetString octetAddress(QByteArray(_bacAddress.macPtr(), _bacAddress.macAddrLength()));
    ret = octetAddress.toRaw(actualPtr, buffLength);
    if (ret < 0)
        return -2;
    total += ret;

    return total;
}

qint32 Address::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 Address::fromRaw(BacnetTagParser &parser)
{
    qint32 ret;
    qint32 total;
    bool convOk;

    //parse network number
    ret = parser.parseNext();
    quint32 netNum = parser.toUInt(&convOk);
    if (ret < 0 || !parser.isApplicationTag(AppTags::UnsignedInteger) || !convOk) {
        return -BacnetError::CodeMissingRequiredParameter;
    }
    total += ret;

    //parse MAC address
    ret = parser.parseNext();
    QByteArray octetAddress = parser.toByteArray(&convOk);
    if (ret < 0 || !parser.isApplicationTag(AppTags::OctetString) || !convOk) {
        return -BacnetError::CodeMissingRequiredParameter;
    }
    total += ret;

    _bacAddress.setNetworkNum(netNum);
    _bacAddress.macAddressFromRaw(octetAddress.constData(), octetAddress.size());

    return total;
}

qint32 Address::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool Address::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant Address::toInternal()
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType Address::typeId()
{
    return DataType::BACnetAddress;
}

///////////////////////////////////////////////////////////////

Recipient::Recipient():
    _address(0),
    _objectId(0)
{
}

Recipient::~Recipient()
{
    delete _objectId; _objectId = 0;
    delete _address; _address = 0;
}

qint32 Recipient::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    if (0 != _address)
        return _address->toRaw(ptrStart, buffLength);
    else if (0 != _objectId)
        return BacnetCoder::objectIdentifierToRaw(ptrStart, buffLength, *_objectId, false);
    else
        Q_ASSERT(false);
}

qint32 Recipient::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 Recipient::fromRaw(BacnetTagParser &parser)
{
    bool convOkOrCtxt;
    qint32 ret(0);

    ret = parser.parseNext();
    if (ret > 0) {
        if (parser.isContextTag(0)) {
            if (0 == _objectId) _objectId = new ObjectIdStruct;
            if (0 != _address) {
                delete _address;
                _address = 0;
            }
            *_objectId = parser.toObjectId(&convOkOrCtxt);
            if (convOkOrCtxt)
                return ret;
        } else if (parser.isContextTag(1)) {
            if (0 != _objectId) {
                delete _objectId;
                _objectId = 0;
            }
            if (0 == _address) {
                _address = new Address();
                return _address->fromRaw(parser);
            }
        }
    } else {
        //nothing matches requirements
        return -BacnetError::CodeMissingRequiredParameter;
    }

    return -BacnetError::CodeInconsistentParameters;
}

qint32 Recipient::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool Recipient::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant Recipient::toInternal()
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType Recipient::typeId()
{
    return DataType::BACnetRecipient;
}


