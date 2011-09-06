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
    quint32 identifier = (objIdentifier.objectType << 22) | objIdentifier.instanceNum;
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

    memcpy(ptrStart + bytesUsed, &tempData[0], bytesUsed);
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

    memcpy(ptrStart + bytesUsed, &tempData[0], bytesUsed);
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
