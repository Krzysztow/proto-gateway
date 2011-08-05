#ifndef BACNETCODER_H
#define BACNETCODER_H

class BacnetCoder
{
public:
    BacnetCoder();

    enum BacnetTags {
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
        ASHRAE2         = 0x0f
    };

    enum CharacterSet {
        AnsiX3_4 = 0x00,
        IbmDbcs = 0x01,
        JisC6266 = 0x02,
        ISO10646 = 0x03
    };
};

class BacnetReadProperty
{
public:
    qint16 fromRaw(quint8 *dataPtr, quint16 lengthLeft);
};

#endif // BACNETCODER_H
