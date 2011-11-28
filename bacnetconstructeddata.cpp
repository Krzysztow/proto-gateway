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

void ObjectTypesSupported::addObjectsSupported(QList<BacnetObjectTypeNS::ObjectType> objSupported)
{
    foreach (BacnetObjectTypeNS::ObjectType supType, objSupported) {
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
    Q_UNUSED(value);
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
    Q_UNUSED(value);
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
PropertyReference::PropertyReference(BacnetPropertyNS::Identifier propId, quint32 arrayIdx):
        _identifier(propId),
        _arrayIdx(arrayIdx)
{
}

PropertyReference::~PropertyReference(){}

qint32 PropertyReference::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    qint32 ret;
    quint8 *actualPtr(ptrStart);

    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _identifier, true, 0);
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
    qint16 ret, total(0);

    //firstly parse obligatory propertyId
    ret = parser.parseNext();
    if (!parser.isContextTag(0))
        return -1;
    _identifier = (BacnetPropertyNS::Identifier)parser.toUInt(&okOrCtxt);
    if (ret < 0 || !okOrCtxt)
        return -1;
    total += ret;
    //now check if we are provided with property array index. If so, parse it.
    if (parser.nextTagNumber(&okOrCtxt) == 1 && okOrCtxt) {
        ret = parser.parseNext();
        if (ret < 0)
            return -2;
        _arrayIdx = parser.toUInt(&okOrCtxt);
        if (!okOrCtxt)
            return -2;
        total += ret;
    } else
        _arrayIdx = ArrayIndexNotPresent;

    return total;
}

qint32 PropertyReference::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool PropertyReference::setInternal(QVariant &value)
{
    Q_UNUSED(value);
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

BacnetPropertyNS::Identifier PropertyReference::propIdentifier()
{
    return _identifier;
}

quint32 PropertyReference::propArrayIndex()
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

//BacnetAddress &Address::opeartor()
//{
//    return _bacAddress;
//}

const BacnetAddress &Address::address() const
{
    return _bacAddress;
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
    Bacnet::OctetString octetAddress(QByteArray((const char*)_bacAddress.macPtr(), _bacAddress.macAddrLength()));
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
    qint32 total(0);
    bool convOk;

    //parse network number
    ret = parser.parseNext();
    quint32 netNum = parser.toUInt(&convOk);
    if (ret < 0 || !parser.isApplicationTag(AppTags::UnsignedInteger) || !convOk) {
        return -BacnetErrorNS::CodeMissingRequiredParameter;
    }
    total += ret;

    //parse MAC address
    ret = parser.parseNext();
    QByteArray octetAddress = parser.toByteArray(&convOk);
    if (ret < 0 || !parser.isApplicationTag(AppTags::OctetString) || !convOk) {
        return -BacnetErrorNS::CodeMissingRequiredParameter;
    }
    total += ret;

    _bacAddress.setNetworkNum(netNum);
    _bacAddress.macAddressFromRaw((quint8*)octetAddress.constData(), octetAddress.size());

    return total;
}

qint32 Address::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool Address::setInternal(QVariant &value)
{
    Q_UNUSED(value);
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

Recipient::Recipient(ObjectIdStruct objectId):
    _objectId(new ObjectIdentifier(objectId)),
    _address(0)
{
}

Recipient::Recipient(BacnetAddress &address):
    _objectId(0),
    _address(new Address(address))
{
}

Recipient::~Recipient()
{
    delete _objectId; _objectId = 0;
    delete _address; _address = 0;
}

bool Recipient::operator==(const Recipient &other) const
{
#warning "Needs correction. We have to utilize routing lookup table."
    //! \todo Correct it! May get duplicatied if one specified by object id and other by its address.
    if (this->hasAddress() != other.hasAddress())
        return false;

    //addresses or recipient device id have to be the same
    if ( this->address() == other.address() &&
            this->objId() == other.objId() ) {
        return true;
    }

    return false;
}

bool Recipient::hasAddress() const
{
    Q_ASSERT(_objectId != 0 || _address != 0);
    return (_address != 0);
}

const Address *Recipient::address() const
{
    return _address;
}

const ObjectIdentifier *Recipient::objId() const
{
    return _objectId;
}

const ObjectIdStruct &Recipient::objIdStruct() const
{
    return _objectId->_value;
}


qint32 Recipient::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    if (0 != _address)
        return _address->toRaw(ptrStart, buffLength, 0);
    else if (0 != _objectId)
        return _objectId->toRaw(ptrStart, buffLength, 1);
    else
        Q_ASSERT(false);

    return -1;
}

qint32 Recipient::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 Recipient::fromRaw(BacnetTagParser &parser)
{
    qint32 ret(0);

    ret = parser.parseNext();
    if (ret > 0) {
        if (parser.isContextTag(0)) {
            if (0 == _objectId) _objectId = new ObjectIdentifier();
            if (0 != _address) {
                delete _address;
                _address = 0;
            }
            return _objectId->fromRaw(parser);
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
        return -BacnetErrorNS::CodeMissingRequiredParameter;
    }

    return -BacnetErrorNS::CodeInconsistentParameters;
}

qint32 Recipient::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool Recipient::setInternal(QVariant &value)
{
            Q_UNUSED(value);
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

///////////////////////////////////////////////////////////////////////

qint32 ObjectPropertyReference::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint8 *actualPtr(ptrStart);
    qint32 total(0), ret(0);

    //encode object identifier
    ret = _objId.toRaw(actualPtr, buffLength, 0);
    if (ret < 0)
        return ret;
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //encode property ID
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _propId, true, 1);
    if (ret < 0)
        return ret;
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //encode property array index, if existing
    if (_arrayIdx != ArrayIndexNotPresent) {
        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _arrayIdx, true, 2);
        if (ret < 0)
            return ret;
        total += ret;
        actualPtr += ret;
        buffLength -= ret;
    }

    return total;
}

qint32 ObjectPropertyReference::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 ObjectPropertyReference::fromRaw(BacnetTagParser &parser)
{
    qint32 ret(0), total(0);
    bool convOkOrCtxt;

    //decode object Identifier
    ret = _objId.fromRaw(parser, 0);
    if (ret < 0)
        return ret;

    //decode property identifier
    ret = parser.parseNext();
    _propId = (BacnetPropertyNS::Identifier)parser.toUInt(&convOkOrCtxt);
    if (ret < 0 || !parser.isContextTag(1))
        return -BacnetErrorNS::CodeMissingRequiredParameter;
    else if (!convOkOrCtxt)
        return -BacnetErrorNS::CodeInvalidDataType;
    total += ret;

    //decode proeprty array index, if present
    if (parser.hasNext() && parser.nextTagNumber(&convOkOrCtxt) == 2) {
        ret = parser.parseNext();
        _arrayIdx = parser.toUInt(&convOkOrCtxt);
        if (ret < 0 || !convOkOrCtxt)
            return -BacnetErrorNS::CodeInconsistentParameters;

        total += ret;
    } else
        _arrayIdx = ArrayIndexNotPresent;

    return total;
}

qint32 ObjectPropertyReference::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool ObjectPropertyReference::setInternal(QVariant &value)
{
    Q_UNUSED(value);
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant ObjectPropertyReference::toInternal()
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType ObjectPropertyReference::typeId()
{
    return DataType::BACnetObjectPropertyReference;
}

ObjectIdentifier &ObjectPropertyReference::objId()
{
    return _objId;
}

BacnetPropertyNS::Identifier ObjectPropertyReference::propId()
{
    return _propId;
}

quint32 ObjectPropertyReference::arrayIdx()
{
    return _arrayIdx;
}

ObjectPropertyReference::ObjectPropertyReference(ObjectIdentifier &objId, BacnetPropertyNS::Identifier propId, quint32 arrayIdx):
    _objId(objId),
    _propId(propId),
    _arrayIdx(arrayIdx)
{
}

bool ObjectPropertyReference::compareParameters(ObjectIdentifier &objId, BacnetPropertyNS::Identifier propId, quint32 arrayIdx) const
{
    return ( (objId == _objId) && (propId == _propId) && (arrayIdx == _arrayIdx) );
}
///////////////////////////////////////////////////////////////////////////////////////////////////

RecipientProcess::RecipientProcess(BacnetAddress &address, quint32 processId):
    _recipient(address),
    _procId(processId)
{

}

RecipientProcess::RecipientProcess(ObjectIdStruct objectId, quint32 processId):
    _recipient(objectId),
    _procId(processId)
{

}

quint32 RecipientProcess::processId()
{
    return _procId.value();
}

bool RecipientProcess::operator==(const RecipientProcess &other) const
{
    return ( (this->_procId == other._procId)/* &&
             (this->_recipient == other._recipient)*/);
}

bool RecipientProcess::compare(BacnetAddress &address, quint32 processId)
{
    Q_ASSERT(processId != 0);
    if (_recipient.address() == 0) {
        qDebug("%s : address not specified!", __PRETTY_FUNCTION__);
        return false;
    }

    return ( (processId == _procId.value()) && (*(_recipient.address()) == address) );
}

bool RecipientProcess::compare(ObjectIdStruct &objId, quint32 processId)
{
    Q_ASSERT(processId != 0);
    if (_recipient.objId() == 0) {
        qDebug("%s : objId not specified!", __PRETTY_FUNCTION__);
        return false;
    }

    return ( (processId == _procId.value()) && (*(_recipient.objId()) == objId) );
}

bool RecipientProcess::recipientHasAddress()
{
    return _recipient.hasAddress();
}

const ObjectIdentifier *RecipientProcess::recipientObjId() const
{
    return _recipient.objId();
}

const Address *RecipientProcess::recipientAddress() const
{
    return _recipient.address();
}

qint32 RecipientProcess::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint32 total(0);
    qint32 ret(0);
    quint8 *actualPtr(ptrStart);

    //encode recipient
    ret = _recipient.toRaw(actualPtr, buffLength, 0);
    if (ret < 0)
        return ret;
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //encode process id
    ret = _procId.toRaw(actualPtr, buffLength, 1);
    if (ret < 0)
        return ret;
    total += ret;
//    actualPtr += ret;
//    buffLength -= ret;

    return total;
}

qint32 RecipientProcess::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 RecipientProcess::fromRaw(BacnetTagParser &parser)
{
    qint32 total(0);
    qint32 ret(0);

    //decode recipient
    ret = _recipient.fromRaw(parser, 0);
    if (ret < 0) {
        qDebug("%s : Can't parse recipient", __PRETTY_FUNCTION__);
        return BacnetErrorNS::CodeMissingRequiredParameter;
    }
    total += ret;

    //decode process id
    ret = _procId.fromRaw(parser, 1);
    if (ret < 0) {
        qDebug("%s : Can't parse process Id", __PRETTY_FUNCTION__);
        return BacnetErrorNS::CodeMissingRequiredParameter;
    }
    total += ret;

    return total;
}

qint32 RecipientProcess::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool RecipientProcess::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant RecipientProcess::toInternal()
{
    Q_ASSERT_X(false, __PRETTY_FUNCTION__, "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType RecipientProcess::typeId()
{
    return DataType::BACnetRecipientProcess;
}
