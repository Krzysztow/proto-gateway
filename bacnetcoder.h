#ifndef BACNETCODER_H
#define BACNETCODER_H

#include <QtCore>
#include "bitfields.h"
#include "bacnetcommon.h"

namespace BacnetCoder
{
    enum CharacterSet {
        AnsiX3_4    = 0x00,
        IbmDbcs     = 0x01,
        JisC6266    = 0x02,
        UCS_4       = 0x03,
        UCS_2       = 0x04,
        ISO_8859_1  = 0x05
    };

    inline bool isContextTag(quint8 *dataPtr) {return (*dataPtr) & BitFields::Bit3;}

    enum WritingError {
        BufferOverrun = -1,
        NotEncodablePrimitiveTag = -2,

        NotApplicationTag = -3,
        ValueIncorrectRange = -5,

        UnknownError = -5
    };

    /** This function encodes the starting tag of the data, as well as the tag length. The function is safe.
      If the buffer is too small to be packed with the tag + length fields and with data length, then the
      \sa WritingError is returned and buffer not overrun.
      */
    qint8 encodeTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, bool isContextTag, quint32 lengthToEncode);
    qint8 encodeAppBool(quint8 *startPtr, quint16 buffLength, bool value);

    inline qint8 encodeContextTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, quint32 lengthToEncode)
    {
        return encodeTagAndLength(startPtr, buffLength, tagToEncode, true, lengthToEncode);
    }

    //! \todo If the performance tests show there is a need to improve this encoding - separate application tag encoding - it may be simplified.
    inline qint8 encodeAppTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, quint32 lengthToEncode)
    {
        return encodeTagAndLength(startPtr, buffLength, tagToEncode, false, lengthToEncode);
    }

    qint32 objectIdentifierToRaw(quint8 *ptrStart, quint16 buffLength, Bacnet::ObjectIdStruct &objIdentifier, bool isContextTag, quint8 tagNumber);
    qint32 uintToRaw(quint8 *ptrStart, quint16 buffLength, quint32 value, bool isContextTag, quint8 tagNumber);
    qint32 sintToRaw(quint8 *ptrStart, quint16 buffLength, quint32 value, bool isContextTag, quint8 tagNumber);
    qint32 closingTagToRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
    qint32 openingTagToRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
    qint32 stringToRaw(quint8 *ptrStart, quint16 buffLength, QString value, bool isContext, quint8 tagNumber, CharacterSet charSet = AnsiX3_4);
    qint32 boolToRaw(quint8 *ptrStart, quint16 buffLength, bool value, bool isContext, quint8 tagNumber);
};


#endif // BACNETCODER_H
