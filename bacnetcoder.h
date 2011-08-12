#ifndef BACNETCODER_H
#define BACNETCODER_H

#include <QtCore>
#include "bitfields.h"

class BacnetCoder
{
public:
    BacnetCoder();

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
};


#endif // BACNETCODER_H
