#include "bacnetprimitivedata.h"

#include "helpercoder.h"
#include "bacnetcommon.h"
#include "bacnettagparser.h"
#include "datavisitor.h"

using namespace Bacnet;

qint32 Null::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);
    return BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength, AppTags::Null, 0);
}

qint32 Null::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    return BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber, 0);
}

qint32 Null::fromRaw(BacnetTagParser &parser)
{
    qint16 ret(parser.parseNext());
    if (ret < 0 || !parser.isApplicationTag(AppTags::Null))
        return -1;
    return ret;
}

qint32 Null::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret(parser.parseNext());
    if (ret < 0 ||
        !parser.isContextTag(tagNum) ||
        (parser.valueLength() != 0) )
        return -1;
    return ret;
}

bool Null::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    if (value.isNull())
        return true;
    Q_ASSERT(false);
    return false;
}

DataType::DataType Null::typeId()
{
    return DataType::Null;
}

QVariant Null::toInternal()
{
    return QVariant();
}

//quint8 Null::typeId()
//{
//    return 0;
//}

//BOOLEAN
qint32 Boolean::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(buffLength >= 1);
    return BacnetCoder::encodeAppBool(ptrStart, buffLength, _value);
}

qint32 Boolean::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    qint8 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber, 1);
    if (ret < 0)
        return ret;
    //successfully encoded and there is additional place for 1 byte of value
    *(ptrStart + ret) = _value;
    return ret + 1;
}

qint32 Boolean::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::Boolean));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::Boolean))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toBoolean(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 Boolean::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toBoolean(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool Boolean::setInternal(QVariant &value)
{
#warning "Why this assertion fails, when QVariant is boolean with false?"
    //Q_ASSERT(!value.isNull());
    bool convOk;
    convOk = value.canConvert(QVariant::Bool);
    _value = value.toBool();
    Q_ASSERT(convOk);
    return convOk;
}

QVariant Boolean::toInternal()
{
    return QVariant(_value);
}

DataType::DataType Boolean::typeId()
{
    return DataType::BOOLEAN;
}

//UNSIGNED INTEGER
UnsignedInteger::UnsignedInteger(quint32 value):
        _value(value)
{
}

void UnsignedInteger::setValue(quint32 value)
{
    _value = value;
}

quint32 UnsignedInteger::value()
{
    return _value;
}

DEFINE_VISITABLE_FUNCTION(UnsignedInteger);

qint32 UnsignedInteger::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);

    return BacnetCoder::uintToRaw(ptrStart, buffLength, _value, false, AppTags::UnsignedInteger);
}

qint32 UnsignedInteger::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    return BacnetCoder::uintToRaw(ptrStart, buffLength, _value, true, tagNumber);
}

qint32 UnsignedInteger::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::UnsignedInteger));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::UnsignedInteger))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toUInt(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 UnsignedInteger::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toUInt(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool UnsignedInteger::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    _value = value.toUInt(&convOk);
    Q_ASSERT(convOk);
    return convOk;
}

QVariant UnsignedInteger::toInternal()
{
    return QVariant(_value);
}

DataType::DataType UnsignedInteger::typeId()
{
    return DataType::Unsigned;
}

//SIGNED INTEGER
qint32 SignedInteger::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);
    return BacnetCoder::sintToRaw(ptrStart, buffLength, _value, false, AppTags::SignedInteger);
}

qint32 SignedInteger::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    return BacnetCoder::sintToRaw(ptrStart, buffLength, _value, true, tagNumber);
}

qint32 SignedInteger::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::SignedInteger));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::SignedInteger))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toInt(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 SignedInteger::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toInt(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool SignedInteger::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    _value = value.toInt(&convOk);
    return convOk;
}

QVariant SignedInteger::toInternal()
{
    return QVariant(_value);
}

DataType::DataType SignedInteger::typeId()
{
    return DataType::Signed;
}

qint32 SignedInteger::value()
{
    return _value;
}

void SignedInteger::setValue(qint32 value)
{
    _value = value;
}

DEFINE_VISITABLE_FUNCTION(SignedInteger);

//REAL
qint32 Real::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);

    qint8 ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength,
                                                   AppTags::Real, 4);
    if (ret < 0)
        return ret;

    Q_ASSERT(4 == HelperCoder::floatToRaw(_value, ptrStart + ret));
    return (ret + HelperCoder::floatToRaw(_value, ptrStart + ret));
}

Real::Real(float value):
        _value(value)
{
}

qint32 Real::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);

    qint8 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength,
                                                   tagNumber, 4);
    if (ret < 0)
        return ret;

    Q_ASSERT(4 == HelperCoder::floatToRaw(_value, ptrStart + ret));
    return (ret + HelperCoder::floatToRaw(_value, ptrStart + ret));
}

qint32 Real::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::Real));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::Real))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toFloat(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 Real::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toFloat(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool Real::setInternal(QVariant &value)
{
    //Q_ASSERT(!value.isNull());
    bool convOk;
    _value = value.toFloat(&convOk);
    //Q_ASSERT(qFuzzyCompare(_value, value.toDouble()));
    return convOk;
}

QVariant Real::toInternal()
{
    return QVariant(_value);
}

DataType::DataType Real::typeId()
{
    return DataType::Real;
}

float Real::value()
{
    return _value;
}

void Real::setValue(float value)
{
    _value = value;
}

DEFINE_VISITABLE_FUNCTION(Real);

//DOUBLE
qint32 Double::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);
    qint8 ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength,
                                                   AppTags::Double, 8);
    if (ret < 0)
        return ret;

    Q_ASSERT(8 == HelperCoder::doubleToRaw(_value, ptrStart + ret));
    return (ret + HelperCoder::doubleToRaw(_value, ptrStart + ret));
}

qint32 Double::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);

    qint8 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength,
                                                   tagNumber, 8);
    if (ret < 0)
        return ret;

    Q_ASSERT(8 == HelperCoder::doubleToRaw(_value, ptrStart + ret));
    return (ret + HelperCoder::doubleToRaw(_value, ptrStart + ret));
}

qint32 Double::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::Double));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::Double))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toDouble(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 Double::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toDouble(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool Double::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    _value = value.toDouble(&convOk);
    Q_ASSERT(value.convert(QVariant::Double));
    Q_ASSERT(qFuzzyCompare(_value, value.toDouble()));
    return convOk;
}

QVariant Double::toInternal()
{
    return QVariant(_value);
}

DataType::DataType Double::typeId()
{
    return DataType::Double;
}

double &Double::value()
{
    return _value;
}

void Double::setValue(double &value)
{
    _value = value;
}

DEFINE_VISITABLE_FUNCTION(Double);

//OCtet String
qint32 OctetString::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    qint32 ret;
    quint16 length = _value.length();
    ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength, AppTags::OctetString, length);
    if (ret < 0)
        return -1;
    //if we are here, we know that there is enough place to encode the data
    ptrStart += ret;
    memcpy(ptrStart, _value.data_ptr(), length);

    return ret + length;
}

qint32 OctetString::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    qint32 ret;
    quint16 length = _value.length();
    ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber, length);
    if (ret < 0)
        return -1;
    //if we are here, we know that there is enough place to encode the data
    ptrStart += ret;
    memcpy(ptrStart, _value.data_ptr(), length);

    return ret + length;
}

qint32 OctetString::fromRaw(BacnetTagParser &parser)
{
    qint32 ret;
    bool convOk;
    ret = parser.parseNext();
    _value = parser.toByteArray(&convOk);
    if (ret<0 || !convOk || parser.isApplicationTag(AppTags::OctetString))
        return -1;
    return ret;
}

qint32 OctetString::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint32 ret;
    bool convOk;
    ret = parser.parseNext();
    _value = parser.toByteArray(&convOk);
    if (ret<0 || !convOk || parser.isContextTag(tagNum))
        return -1;
    return ret;
}

bool OctetString::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    convOk = value.canConvert(QVariant::ByteArray);
    _value = value.toByteArray();
    return convOk;
}

QVariant OctetString::toInternal()
{
    return QVariant(_value);
}

DataType::DataType OctetString::typeId()
{
    return DataType::OctetString;
}

OctetString::OctetString()
{
}

OctetString::OctetString(QByteArray value):
    _value(value)
{
}

void OctetString::setValue(QByteArray value)
{
    _value = value;
}

QByteArray OctetString::value()
{
    return _value;
}

////Character STRING
//qint32 CharacterString::toRaw_helper(quint8 *ptrStart, quint16 buffLength, bool isContext, quint8 tagNumber)
//{
//    Q_CHECK_PTR(ptrStart);
//    quint8 encodedLength(0);
//    qint32 ret;
//
//    switch (_charSet)
//    {
//    case (BacnetCoder::AnsiX3_4): {
//            encodedLength = _value.length();
//            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
//                                                  tagNumber, isContext, encodedLength + 1);
//            if (ret < 0) return ret;
//            QByteArray tempBytes = _value.toAscii();
//            *(ptrStart + ret) = _charSet;
//            memcpy(ptrStart + ret + 1, tempBytes.data(), encodedLength);
//
//            return ret + 1 + encodedLength;
//        }
//    case (BacnetCoder::IbmDbcs): //break;//fall through
//    case (BacnetCoder::JisC6266): {
//            qWarning("This encoding is not supported!");
//            //                Q_ASSERT(false);
//            return -3;//encoding not supported
//        }
//    case (BacnetCoder::UCS_4): {
//            encodedLength = 4 * _value.length();
//            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
//                                                  tagNumber, isContext, encodedLength + 1);            if (ret < 0) return ret;
//            *(ptrStart + ret) = _charSet;
//            QVector<uint> tempBytes = _value.toUcs4();
//            quint32 *destLetterPtr = (quint32*)(ptrStart + ret + 1);
//            const int iterNum = _value.length();
//            for (int i = 0; i<iterNum; ++i) {
//                HelperCoder::uint32ToRaw(tempBytes.at(i), (quint8*)destLetterPtr);
//                ++destLetterPtr;
//            }
//            return ret + 1 + encodedLength;
//        }
//    case (BacnetCoder::UCS_2): {
//            encodedLength = 2 * _value.length();
//            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
//                                                  tagNumber, isContext, encodedLength + 1);
//            if (ret < 0) return ret;
//            *(ptrStart + ret) = _charSet;
//            const ushort *tempBytes = _value.utf16();
//            quint16 *destLetterPtr = (quint16*)(ptrStart + ret + 1);
//            const int iterNum = _value.length();
//            for (int i = 0; i<iterNum; ++i) {
//                HelperCoder::uin16ToRaw(*(tempBytes + i), (quint8*)destLetterPtr);
//                ++destLetterPtr;
//            }
//
//            return ret + 1 + encodedLength;
//            break;
//        }
//    case (BacnetCoder::ISO_8859_1): {
//            encodedLength = _value.length();
//            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
//                                                  tagNumber, isContext, encodedLength + 1);
//            if (ret < 0) return ret;
//            QByteArray tempBytes = _value.toLatin1();
//
//            *(ptrStart + ret) = _charSet;
//            memcpy(ptrStart + ret + 1, tempBytes.data(), encodedLength);
//
//            return ret + 1 + encodedLength;
//        }
//    default:
//        return -3;//encoding not supported
//    }
//}

qint32 CharacterString::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    return BacnetCoder::stringToRaw(ptrStart, buffLength, _value, false, AppTags::CharacterString);
}

qint32 CharacterString::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return BacnetCoder::stringToRaw(ptrStart, buffLength,_value, true, tagNumber);
}

qint32 CharacterString::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::CharacterString));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::CharacterString))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toString(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

//! \todo \note These functions may be extracted to one in case of Primitive types
qint32 CharacterString::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toString(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool CharacterString::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    convOk = value.canConvert(QVariant::String);
    _value = value.toString();
    return convOk;
}

QVariant CharacterString::toInternal()
{
    return QVariant(_value);
}

DataType::DataType CharacterString::typeId()
{
    return DataType::CharacterString;
}

QString &CharacterString::value()
{
    return _value;
}

void CharacterString::setValue(QString &value)
{
    _value = value;
}


//BIT STRING
void BitString::toRaw_helper(quint8 *dataStart)
{
    *dataStart = 8 - _value.size()%8;
    quint8 mask = 0x00;
    //encode bits
    for (int bitNumber = _value.size() - 1; bitNumber >= 0; --bitNumber) {
        if (0x00 == mask) {
            ++dataStart;
            *dataStart = 0x00;
            mask = 0x80;
        }
        if (_value.testBit(bitNumber)) {
            *dataStart |= mask;
        }
        mask >>= 1;
    }
}

qint32 BitString::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);

    quint8 bytesNeeded = _value.size();//in this line, we set bits needed. Later bytes number is computed.
    if (0 != bytesNeeded) {
        bytesNeeded = bytesNeeded/8 + 1;
    }
    ++bytesNeeded;//apart from data bytes there is unused-bits-number field
    qint8 ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength, AppTags::BitString,
                                                    bytesNeeded);
    if (ret < 0)
        return ret;
    //encode number of bits unused
    toRaw_helper(ptrStart + ret);//we don't have to check for the buffer space - we have already done it inside encodeAppTagAndLength
    return (ret + bytesNeeded);
}

qint32 BitString::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);

    quint8 bytesNeeded = _value.size();
    if (0 != bytesNeeded) {
        bytesNeeded = bytesNeeded%8 + 1;
    }
    qint8 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber,
                                                    bytesNeeded + 1);//apart from bytes there is unused-bits-number field
    if (ret < 0)
        return ret;
    //encode number of bits unused
    toRaw_helper(ptrStart + ret);//we don't have to check for the buffer space - we have already done it inside encodeAppTagAndLength
    return (ret + bytesNeeded);
}

qint32 BitString::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::BitString));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::BitString))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toBitArray(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 BitString::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toBitArray(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool BitString::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    convOk = value.canConvert(QVariant::BitArray);
    _value = value.toBitArray();
    return convOk;
}

QVariant BitString::toInternal()
{
    return QVariant(_value);
}

DataType::DataType BitString::typeId()
{
    return DataType::BitString;
}

QBitArray &BitString::value()
{
    return _value;
}

//ENUMERATRED - similar to AppTags::UnsignedInteger
Enumerated::Enumerated(quint32 enumValue):
        _value(enumValue)
{
}

qint32 Enumerated::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);

    quint8 tempData[4];
    quint8 bytesUsed = HelperCoder::uint32ToVarLengthRaw(&tempData[0], _value);
    qint8 ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength, AppTags::Enumerated,
                                                    bytesUsed);
    if (ret < 0)
        return ret;
    Q_ASSERT(bytesUsed <= 4);
    memcpy(ptrStart + bytesUsed, &tempData[0], bytesUsed);

    return (ret + bytesUsed);
}

qint32 Enumerated::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    quint8 tempData[4];
    quint8 bytesUsed = HelperCoder::uint32ToVarLengthRaw(&tempData[0], _value);
    qint8 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber,
                                                    bytesUsed);
    if (ret < 0)
        return ret;
    Q_ASSERT(bytesUsed <= 4);
    memcpy(ptrStart + bytesUsed, &tempData[0], bytesUsed);

    return (ret + bytesUsed);
}

qint32 Enumerated::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::Enumerated));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::Enumerated))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toEumerated(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 Enumerated::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toEumerated(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool Enumerated::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    _value = value.toUInt(&convOk);
    return convOk;
}

QVariant Enumerated::toInternal()
{
    return QVariant(_value);
}

DataType::DataType Enumerated::typeId()
{
    return DataType::Enumerated;
}

//DATE
void Date::toRaw_helper(quint8 *dataStart)
{
    *dataStart = (_value.year() - 1900);
    ++dataStart;
    *dataStart = _value.month();
    ++dataStart;
    *dataStart = _value.day();
    ++dataStart;
    *dataStart = _value.dayOfWeek();
}

qint32 Date::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);

    qint16 ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength, AppTags::Date, 4);
    if (ret <= 0)
        return ret;
    Q_ASSERT(ret > 0);

    toRaw_helper(ptrStart + ret);
    return (ret + 4);
}

qint32 Date::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    qint16 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber, 4);
    if (ret <= 0)
        return ret;
    Q_ASSERT(ret > 0);

    toRaw_helper(ptrStart + ret);
    return (ret + 4);
}

qint32 Date::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::Date));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::Date))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toDate(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 Date::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toDate(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool Date::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    convOk = value.canConvert(QVariant::Date);
    _value = value.toDate();
    return convOk;
}

QVariant Date::toInternal()
{
    return QVariant(_value);
}

DataType::DataType Date::typeId()
{
    return DataType::Date;
}

//TIME
void Time::toRaw_helper(quint8 *dataStart)
{
    *dataStart = _value.hour();
    ++dataStart;
    *dataStart = _value.minute();
    ++dataStart;
    *dataStart = _value.second();
    ++dataStart;
    *dataStart = _value.msec()/100;
}

qint32 Time::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(ptrStart);

    qint16 ret = BacnetCoder::encodeAppTagAndLength(ptrStart, buffLength, AppTags::Time, 4);
    if (ret <= 0)
        return ret;
    Q_ASSERT(ret > 0);

    toRaw_helper(ptrStart + ret);
    return (ret + 4);
}

qint32 Time::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(tagNumber <= 254);

    qint16 ret = BacnetCoder::encodeContextTagAndLength(ptrStart, buffLength, tagNumber, 4);
    if (ret <= 0)
        return ret;
    Q_ASSERT(ret > 0);

    toRaw_helper(ptrStart + ret);
    return (ret + 4);
}

qint32 Time::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::Time));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::Time))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toTime(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 Time::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toTime(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool Time::setInternal(QVariant &value)
{
    Q_ASSERT(!value.isNull());
    bool convOk;
    convOk = value.canConvert(QVariant::Time);
    _value = value.toTime();
    return convOk;
}

QVariant Time::toInternal()
{
    return QVariant(_value);
}

DataType::DataType Time::typeId()
{
    return DataType::Time;
}

//OBJECT IDENTIFIER
ObjectIdentifier::ObjectIdentifier(BacnetObjectTypeNS::ObjectType type, quint32 instanceNum)
{
    _value.instanceNum = instanceNum;
    _value.objectType = type;
}

ObjectIdentifier::ObjectIdentifier(Bacnet::ObjectIdStruct &objId):
       _value(objId)
{
}

ObjectIdentifier::ObjectIdentifier(quint32 objectIdNum):
    _value(numToObjId(objectIdNum))
{
}

quint32 ObjectIdentifier::objectIdNum() const
{
    return objIdToNum(_value);
}

void ObjectIdentifier::setObjectIdNum(quint32 objIdNum)
{
    _value = numToObjId(objIdNum);
}

void ObjectIdentifier::setObjectId(ObjectIdStruct &value)
{
    _value = value;
}

qint32 ObjectIdentifier::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    return BacnetCoder::objectIdentifierToRaw(ptrStart, buffLength, _value, false, AppTags::BacnetObjectIdentifier);
}

qint32 ObjectIdentifier::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return BacnetCoder::objectIdentifierToRaw(ptrStart, buffLength, _value, true, tagNumber);
}

qint32 ObjectIdentifier::fromRaw(BacnetTagParser &parser)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isApplicationTag(AppTags::BacnetObjectIdentifier));
    if (ret >= 0) {
        if (!parser.isApplicationTag(AppTags::BacnetObjectIdentifier))
            return BacnetTagParser::AppTagNotRequestedType;
        bool ok;
        _value = parser.toObjectId(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

qint32 ObjectIdentifier::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    qint16 ret = parser.parseNext();
    Q_ASSERT(parser.isContextTag(tagNum));
    if (ret >= 0) {
        if (!parser.isContextTag(tagNum))
            return BacnetTagParser::CtxTagNotRequested;
        bool ok;
        _value = parser.toObjectId(&ok);
        if (!ok)
            return parser.error();
    }
    return ret;
}

bool ObjectIdentifier::setInternal(QVariant &value)
{
    //this should be never used with QVariant type - would be insane!
    Q_UNUSED(value);
    Q_ASSERT_X(false, "bacnetprimitivedata.cpp", "You don't want to store BacnetId ");
    return false;
}

QVariant ObjectIdentifier::toInternal()
{
    return QVariant((quint32)(_value.objectType << 22) | (_value.instanceNum & 0x3fffff));
}

DataType::DataType ObjectIdentifier::typeId()
{
    return DataType::BACnetObjectIdentifier;
}

//
BacnetDataBaseDeletable::BacnetDataBaseDeletable(BacnetDataInterface *shared):
        _value(shared)
{
}

BacnetDataBaseDeletable::~BacnetDataBaseDeletable()
{
    delete _value;
}

qint32 BacnetDataBaseDeletable::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_CHECK_PTR(_value);
    return _value->toRaw(ptrStart, buffLength);
}

qint32 BacnetDataBaseDeletable::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return _value->toRaw(ptrStart, buffLength, tagNumber);
}

qint32 BacnetDataBaseDeletable::fromRaw(BacnetTagParser &parser)
{
    Q_UNUSED(parser);
    Q_ASSERT_X(false, "BacnetDataBaseDeletable::fromRaw()", "This method is not to be called!");
    return BacnetTagParser::AppTagNotRequestedType;
}

qint32 BacnetDataBaseDeletable::fromRaw(BacnetTagParser &parser, quint8 tagNumber)
{
    Q_UNUSED(parser);
    Q_UNUSED(tagNumber);
    Q_ASSERT_X(false, "BacnetDataBaseDeletable::fromRaw()", "This method is not to be called!");
    return BacnetTagParser::AppTagNotRequestedType;
}

bool BacnetDataBaseDeletable::setInternal(QVariant &value)
{
    Q_UNUSED(value);
    Q_ASSERT_X(false, "bacnetprimitivedata.cpp", "You shouldn't call this funciton!");
    return false;
}

QVariant BacnetDataBaseDeletable::toInternal()
{
    return _value->toInternal();
}

DataType::DataType BacnetDataBaseDeletable::typeId()
{
    return _value->typeId();
}

//BACNET List
BacnetList::BacnetList()
{

}

BacnetList::BacnetList(QList<BacnetDataInterfaceShared> &value):
    _value(value)
{
}

BacnetList::~BacnetList()
{
}

qint32 BacnetList::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint8 *actualStartPtr = ptrStart;
    qint16 ret(0);
    foreach (BacnetDataInterfaceShared bIt, _value) {
        Q_CHECK_PTR(bIt);
        ret = bIt->toRaw(actualStartPtr, buffLength);
        if (ret < 0)
            return ret;

        actualStartPtr += ret;
        buffLength -= ret;
    }

    return (actualStartPtr - ptrStart);
}

qint32 BacnetList::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    quint8 *actualStartPtr = ptrStart;
    qint16 ret(0);
    foreach (BacnetDataInterfaceShared bIt, _value) {
        Q_CHECK_PTR(bIt);
        ret = bIt->toRaw(actualStartPtr, buffLength, tagNumber);
        if (ret < 0)
            return ret;

        actualStartPtr += ret;
        buffLength -= ret;
    }

    return (actualStartPtr - ptrStart);
}

//! \todo The list should be translated to sequence and sequence of
qint32 BacnetList::fromRaw(BacnetTagParser &parser)
{
#warning "Not implemented, yet. Must be!"
    return -1;
}

qint32 BacnetList::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
#warning "Not implemented, yet. Must be!"
    return -1;
}

bool BacnetList::addElement(BacnetDataInterface *value)
{
    Q_CHECK_PTR(value);
    if (DataType::InvalidType != _storedType) {
        if (value->typeId() != _storedType) {
            Q_ASSERT(false);
            return false;
        }
    } else
        _storedType = value->typeId();

    _value.append(BacnetDataInterfaceShared(value));
    return true;
}

bool BacnetList::setInternal(QVariant &value)
{
    Q_UNUSED(value);
    Q_ASSERT_X(false, "bacnetprimitivedata.cpp", "You shouldn't call this funciton!");
    return false;
}

QVariant BacnetList::toInternal()
{
    QList<QVariant> _list;
    foreach (Bacnet::BacnetDataInterfaceShared data, _value) {
        _list.append(data->toInternal());
    }

    return QVariant(_list);
}

DataType::DataType BacnetList::typeId()
{
    return DataType::BACnetList;
}

void BacnetList::setStoredType(DataType::DataType type)
{
    _storedType = type;
}

DataType::DataType BacnetList::storedType()
{
    return _storedType;
}

//BACNET ARRAY
BacnetArray::BacnetArray():
        BacnetList()
{
}

BacnetArray::BacnetArray(QList<BacnetDataInterfaceShared> &value):
        BacnetList(value)
{
}

//BacnetDataInterface *BacnetArray::createElementAt(quint8 position)
//{
//    if (0 == position)
//        return new UnsignedInteger(_value.size());

//    if (position >= _value.size()) {
//        return 0;
//    }

//    return new BacnetDataBaseDeletable(_value.at(position-1));
//}

DataType::DataType BacnetArray::typeId()
{
    return DataType::BACnetArray;
}
