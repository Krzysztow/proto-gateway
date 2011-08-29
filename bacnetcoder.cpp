#include "bacnetcoder.h"
#include "helpercoder.h"

using namespace BacnetCoder;

qint8 BacnetCoder::encodeAppBool(quint8 *startPtr, quint16 buffLength, bool value)
{
    qint8 ret = encodeTagAndLength(startPtr, buffLength, BacnetCoder::Boolean, false, 0);
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
