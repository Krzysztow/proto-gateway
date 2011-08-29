#ifndef BACNETCODER_H
#define BACNETCODER_H

#include <QtCore>
#include "bitfields.h"

namespace BacnetCoder
{
    enum BacnetTags {
        InvalidTag      = -1,

        Null            = 0x00,
        Boolean         = 0x01,
        UnsignedInteger = 0x02,
        SignedInteger   = 0x03,
        Real            = 0x04,
        Double          = 0x05,
        OctetString     = 0x06,
        CharacterString = 0x07,
        BitString       = 0x08,
        Enumerated      = 0x09,
        Date            = 0x0a,
        Time            = 0x0b,
        BacnetObjectIdentifier = 0x0c,
        ASHRAE0         = 0x0d,
        ASHRAE1         = 0x0e,
        ASHRAE2         = 0x0f,
        LastAshraeTag   = ASHRAE2,

        ExtendedTagNumber = 0xff
    };

    enum CharacterSet {
        AnsiX3_4    = 0x00,
        IbmDbcs     = 0x01,
        JisC6266    = 0x02,
        UCS_4       = 0x03,
        UCS_2       = 0x04,
        ISO_8859_1  = 0x05
    };

    static inline bool isContextTag(quint8 *dataPtr) {return (*dataPtr) & BitFields::Bit3;}

    enum WritingError {
        BufferOverrun = -1,
        NotEncodablePrimitiveTag = -2
    };

    /** This function encodes the starting tag of the data, as well as the tag length. The function is safe.
      If the buffer is too small to be packed with the tag + length fields and with data length, then the
      \sa WritingError is returned and buffer not overrun.
      */
    qint8 encodeTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, bool isContextTag, quint32 lengthToEncode);
    qint8 encodeAppBool(quint8 *startPtr, quint16 buffLength, bool value);

    static inline qint8 encodeContextTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, quint32 lengthToEncode)
    {
        return encodeTagAndLength(startPtr, buffLength, tagToEncode, true, lengthToEncode);
    }

    //! \todo If the performance tests show there is a need to improve this encoding - separate application tag encoding - it may be simplified.
    static inline qint8 encodeAppTagAndLength(quint8 *startPtr, quint16 buffLength, quint8 tagToEncode, quint32 lengthToEncode)
    {
        return encodeTagAndLength(startPtr, buffLength, tagToEncode, false, lengthToEncode);
    }

};


#endif // BACNETCODER_H
