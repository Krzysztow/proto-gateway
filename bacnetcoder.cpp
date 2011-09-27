#include "bacnetcoder.h"
#include "helpercoder.h"

#define BACNET_OPENING_TAG 0x06
#define BACNET_CLOSING_TAG 0x07

using namespace Bacnet;

qint8 BacnetCoder::encodeAppBool(quint8 *startPtr, quint16 buffLength, bool value)
{
    qint8 ret = encodeTagAndLength(startPtr, buffLength, AppTags::Boolean, false, 0/*will be overwritten*/);
    Q_ASSERT(1 == ret);
    *startPtr |= value;
    return ret;
}

qint8 BacnetCoder::encodeTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, bool isContextTag, quint32 lengthToEncode)
{
    quint8 lengthFieldShift = 0;
    Q_ASSERT(!isContextTag ? (tagToEncode < 16) : true);

    Q_ASSERT(buffLength >= 1);
    if (0 == buffLength) return BacnetCoder::BufferOverrun;

    if (tagToEncode <= 14) {//tag is not extended
        *startPtr = (tagToEncode << 4) | (isContextTag << 3);
    } else {//the tag is extended
        Q_ASSERT(buffLength >= 2);
        if (buffLength < 2) return BacnetCoder::BufferOverrun;
        *startPtr = 0xf0 | (isContextTag << 3);
        *(startPtr + 1) = tagToEncode;
        ++lengthFieldShift;
    }

    if (lengthToEncode <= 4) {
        *startPtr |= lengthToEncode;
        ++lengthFieldShift;
    } else  {
        *startPtr |= 0x05;//B'101'
        if (lengthToEncode <= 253) {
            ++lengthFieldShift;
            Q_ASSERT( 1 + lengthFieldShift + lengthToEncode <= buffLength);
            if (1 + lengthFieldShift + lengthToEncode > buffLength) return BacnetCoder::BufferOverrun;
            *(startPtr + lengthFieldShift) = (quint8) lengthToEncode;
            ++lengthFieldShift;
        } else if (lengthToEncode <= 65535) {
            ++lengthFieldShift;
            Q_ASSERT(2 + 2 + lengthFieldShift + lengthToEncode <= buffLength);//additional 254 is added and 2 bytes of length encoded
            if (2 + 2 + lengthFieldShift + lengthToEncode > buffLength) return BacnetCoder::BufferOverrun;
            *(startPtr + lengthFieldShift) = 254;
            ++lengthFieldShift;
            lengthFieldShift += HelperCoder::uin16ToRaw((quint16)lengthToEncode, (startPtr + lengthFieldShift));
        } else if (lengthToEncode <= (0xffffffff - 1)) {
            ++lengthFieldShift;
            Q_ASSERT(2 + 4 + lengthFieldShift + lengthToEncode <= buffLength);//additional 255 is added and 4 bytes encoding length
            if (2 + 4 + lengthFieldShift + lengthToEncode > buffLength) return BacnetCoder::BufferOverrun;
            *(startPtr + lengthFieldShift) = 255;
            ++lengthFieldShift;
            lengthFieldShift += HelperCoder::uint32ToRaw(lengthToEncode, startPtr + lengthFieldShift);
        } else
            return BacnetCoder::NotEncodablePrimitiveTag;
    }

    return lengthFieldShift;
}

qint32 BacnetCoder::boolToRaw(quint8 *ptrStart, quint16 buffLength, bool value, bool isContext, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    if (isContext) {
        qint16 ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength, tagNumber, isContextTag, 1);
        if (ret <= 0)
            return ret;
        ++ptrStart;
        *ptrStart |= (quint8)value;
        return 2;
    } else {
        qint16 ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength, tagNumber, isContextTag, 0);//no overhead for additional data fields.
        if (ret <= 0)
            return ret;
        *ptrStart |= (quint8)value;
        return 1;
    }
}

qint32 BacnetCoder::objectIdentifierToRaw(quint8 *ptrStart, quint16 buffLength, Bacnet::ObjectIdStruct &objIdentifier, bool isContextTag, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);

    qint16 ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength, tagNumber, isContextTag, 4);
    if (ret <= 0)
        return ret;
    Q_ASSERT(ret > 0);
    Q_ASSERT(objIdentifier.objectType != BacnetObjectType::Undefined);
    Q_ASSERT(objIdentifier.objectType <= 0x03ff);
    Q_ASSERT(objIdentifier.instanceNum <= 0x3FFFFF);
    buffLength -= ret;
    if (buffLength < 4)
        return BacnetCoder::BufferOverrun;
    quint32 identifier = objIdToNum(objIdentifier);
    HelperCoder::uint32ToRaw(identifier, ptrStart + ret);

    return (ret + 4);
}

qint32 BacnetCoder::uintToRaw(quint8 *ptrStart, quint16 buffLength, quint32 value, bool isContextTag, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);

    quint8 tempData[4];
    quint8 bytesUsed = HelperCoder::uint32ToVarLengthRaw(&tempData[0], value);
    qint8 ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength, tagNumber, isContextTag, bytesUsed);
    if (ret < 0)
        return ret;
    Q_ASSERT(bytesUsed <= 4);
    if (ret + bytesUsed > buffLength)
        return BacnetCoder::BufferOverrun;

    memcpy(ptrStart + ret, &tempData[0], bytesUsed);
    return (ret + bytesUsed);
}

qint32 BacnetCoder::sintToRaw(quint8 *ptrStart, quint16 buffLength, quint32 value, bool isContextTag, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);

    quint8 tempData[4];
    quint8 bytesUsed = HelperCoder::uint32ToVarLengthRaw(&tempData[0], value);
    qint8 ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength, isContextTag, tagNumber, bytesUsed);
    if (ret < 0)
        return ret;
    Q_ASSERT(bytesUsed <= 4);
    if (bytesUsed + ret > buffLength)
        return BacnetCoder::BufferOverrun;

    memcpy(ptrStart + ret, &tempData[0], bytesUsed);
    return (ret + bytesUsed);
}

qint32 BacnetCoder::openingTagToRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(buffLength >= 1);

    qint32 ret = encodeContextTagAndLength(ptrStart, buffLength, tagNumber, 0);
    Q_ASSERT(1 == ret);
    *ptrStart |= BACNET_OPENING_TAG;
    return ret;
}

qint32 BacnetCoder::closingTagToRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_CHECK_PTR(ptrStart);
    Q_ASSERT(buffLength >= 1);

    qint32 ret = encodeContextTagAndLength(ptrStart, buffLength, tagNumber, 0);
    Q_ASSERT(1 == ret);
    *ptrStart |= BACNET_CLOSING_TAG;
    return ret;
}

qint32 BacnetCoder::stringToRaw(quint8 *ptrStart, quint16 buffLength, QString value, bool isContext, quint8 tagNumber, CharacterSet charSet)
{
    Q_CHECK_PTR(ptrStart);
    quint8 encodedLength(0);
    qint32 ret;

    switch (charSet)
    {
    case (BacnetCoder::AnsiX3_4): {
            encodedLength = value.length();
            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
                                                  tagNumber, isContext, encodedLength + 1);
            if (ret < 0) return ret;
            QByteArray tempBytes = value.toAscii();
            *(ptrStart + ret) = charSet;
            memcpy(ptrStart + ret + 1, tempBytes.data(), encodedLength);

            return ret + 1 + encodedLength;
        }
    case (BacnetCoder::IbmDbcs): //break;//fall through
    case (BacnetCoder::JisC6266): {
            qWarning("This encoding is not supported!");
            //                Q_ASSERT(false);
            return -3;//encoding not supported
        }
    case (BacnetCoder::UCS_4): {
            encodedLength = 4 * value.length();
            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
                                                  tagNumber, isContext, encodedLength + 1);            if (ret < 0) return ret;
            *(ptrStart + ret) = charSet;
            QVector<uint> tempBytes = value.toUcs4();
            quint32 *destLetterPtr = (quint32*)(ptrStart + ret + 1);
            const int iterNum = value.length();
            for (int i = 0; i<iterNum; ++i) {
                HelperCoder::uint32ToRaw(tempBytes.at(i), (quint8*)destLetterPtr);
                ++destLetterPtr;
            }
            return ret + 1 + encodedLength;
        }
    case (BacnetCoder::UCS_2): {
            encodedLength = 2 * value.length();
            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
                                                  tagNumber, isContext, encodedLength + 1);
            if (ret < 0) return ret;
            *(ptrStart + ret) = charSet;
            const ushort *tempBytes = value.utf16();
            quint16 *destLetterPtr = (quint16*)(ptrStart + ret + 1);
            const int iterNum = value.length();
            for (int i = 0; i<iterNum; ++i) {
                HelperCoder::uin16ToRaw(*(tempBytes + i), (quint8*)destLetterPtr);
                ++destLetterPtr;
            }

            return ret + 1 + encodedLength;
            break;
        }
    case (BacnetCoder::ISO_8859_1): {
            encodedLength = value.length();
            ret = BacnetCoder::encodeTagAndLength(ptrStart, buffLength,
                                                  tagNumber, isContext, encodedLength + 1);
            if (ret < 0) return ret;
            QByteArray tempBytes = value.toLatin1();

            *(ptrStart + ret) = charSet;
            memcpy(ptrStart + ret + 1, tempBytes.data(), encodedLength);

            return ret + 1 + encodedLength;
        }
    default:
        return -3;//encoding not supported
    }
}

