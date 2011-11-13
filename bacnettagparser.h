#ifndef BACNETTAGPARSER_H
#define BACNETTAGPARSER_H

#include <QtCore>
#include <QBitArray>
#include "bacnetcoder.h"
#include "helpercoder.h"
#include "bacnetcommon.h"
#include "bacnetdata.h"

//this is used for detection of open/close tag. They have to be context specific (Bit3 is set to 1) and
//110 when opening, whereas 111 when closing tag is encoded
#define OPEN_CLOSE_TAG_MASK (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2 | BitFields::Bit3)
#define OPEN_TAG (0x00 | BitFields::Bit1 | BitFields::Bit2 | BitFields::Bit3)
#define CLOSE_TAG (0x00 | BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2 | BitFields::Bit3)


namespace Bacnet {

class BacnetTagParser
{
public:
    /**The error enumeration that may happen while parsing:
      -NoError                  - no error occured,
      -BufferOverrun            - buffer left length is insufficient to parse tags and its values,
      -AppTagNotRequestedType   - application type tag is being tried to be converted into wrong type with to*() function,
      -ContextValueWrongLength  - the context type value is trying to be converted into value of not matching length.
      */
    enum BacnetTagParserError {
        NoError = 0,

        BufferOverrun = -1,
        AppTagNotRequestedType = -2,
        CtxTagNotRequested      = -3,
        ContextValueWrongLength = -4
    };

    BacnetTagParser(quint8 *data, quint16 length):
            _trackedData(data),
            _copiedData(0),
            _leftLength(length),
            _actualTagPtr(data),
            _valuePtr(_actualTagPtr),//note this is a trick to be consistent inside parseNext function
            _valueLength(0),
            _error(NoError)
    {
    }

    //
    static qint16 parseStructuredData(BacnetTagParser &bParser,
                                      BacnetObjectType::ObjectType objType, BacnetProperty::Identifier propId, quint32 arrayIndex,
                                      quint8 tagToParse, Bacnet::BacnetDataInterfaceShared &resultData);

    //default copy constructor is ok, when we need to have our own copy of data, call copyData()
    ~BacnetTagParser()
    {
        delete []_copiedData;
    }

    /**
      Sets the data to be parsed.
      \note Keep in mind that the buffer data cannot be altered while parsing. To be able to do so, use \sa copyData.
      */
    void setData(quint8 *data, quint16 length);

    /**
      This function copies all the data from buffer passed by setData. From now on, the original data may be destroyed or
      altered and this will have no negative influence on parsing state.
      */
    void copyData();

    /**
      Parses next bacnet token (starting at tag octet). If there is nothing to be parsed - returns 0. If there is not enough data in a buffer - returns BacnetTagParserError.
      */
    qint16 parseNext();

    inline bool isContextTag()
    {
        return _isContextTag;
    }

    /** Returns length of the entire token - with tag (be it simple or extended, length fields and data). If the
        tag is opening or closing one, it will return it's length of 1.
      */
    quint16 actualTagAndDataLength();

    //! Cheks if the token recently parsed is a context one and of number tagNumber.
    bool isContextTag(qint16 tagNumber);

    //! Ckecks if the token recently parsed is an application one and of number tag tagNumber (remember there are only 15 application tags!).
    bool isApplicationTag(AppTags::BacnetTags tag);

    //! Returns next tag number or 0 if there is no next tag, or negative value in case of error (buffer overrun).
    qint16 nextTagNumber(bool *isContextTag);

    inline qint16 tagNumber() {
        return _tagNum;
    }

    //! Used for conversions from Boolean BACnet type.
    bool toBoolean(bool *ok = 0);

    //! Used for conversions from Unsigned Integer BACnet type.
    quint32 toUInt(bool *ok = 0);

    //! Used for conversions from Signed Integer BACnet type.
    qint32 toInt(bool *ok = 0);

    //! Used for conversions from Real BACnet type.
    float toFloat(bool *ok = 0);

    /** Used for conversions from Double BACnet type.
      */
    double toDouble(bool *ok = 0);

    /** Used for conversions from OctetString BACnet type.
        \note no problem the value is returned - QByteArray is implicitly shared.
      */
    QByteArray toByteArray(bool *ok = 0);

    /** Used to convert BACnet BitString to QBitArray
        \note The first byte in the raw Bacnet frame is the MSB
      */
    QBitArray toBitArray(bool *ok = 0);

    QDate toDate(bool *ok = 0);
    QTime toTime(bool *ok = 0);

    Bacnet::ObjectIdStruct toObjectId(bool *ok = 0);

    /**
      Returns QString with decoded value from token value we are currently at (if the token is string).
      \not that currently, ANSI, UCS-2, UCS-4 and ISO 8859-1 is supported (no JIS C 6226 and DBCS)
      */
    QString toString(bool *ok = 0);

    //! Returns integer value representing enumeration (if the current token is enumerated)
    quint32 toEumerated(bool *ok = 0);

    //! returns true if the tag actually parsed is an opening one.
    inline bool isOpeningTag() {
        //make sure that if it's an opening or closing tag, the class field is set to context
        Q_ASSERT(((*_actualTagPtr & (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2)) >= 0x0e) ? BacnetCoder::isContextTag(_actualTagPtr) : true);
        return (OPEN_TAG == (*_actualTagPtr & OPEN_CLOSE_TAG_MASK));
    }

    bool isOpeningTag(quint8 tag);

    //! Returns true, if the tag actuall parsed is a closing one.
    inline bool isClosingTag() {
        //make sure that if it's an opening or closing tag, the class field is set to context
        Q_ASSERT(((*_actualTagPtr & (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2)) >= 0x0e) ? BacnetCoder::isContextTag(_actualTagPtr) : true);
        return (CLOSE_TAG == (*_actualTagPtr & OPEN_CLOSE_TAG_MASK));
    }

    bool isClosingTag(quint8 tag);

    /**
      Returns true if the initial tag LTV is of value B'110' or B'111', meaning that actually parsed tag is either opening or closing one.
      \warning assumes _actualPtr points at initial octet.
      */
    inline bool isOpeningOrClosingTag() {
        //make sure that if it's an opening or closing tag, the class field is set to context
        Q_ASSERT(((*_actualTagPtr & (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2)) >= 0x0e) ? BacnetCoder::isContextTag(_actualTagPtr) : true);
        return ((*_actualTagPtr & OPEN_CLOSE_TAG_MASK) >= 0x0e);
    }

    quint16 valueLength();

    inline quint8 *actualTagStart() {
        return _actualTagPtr;
    }

    //! Returns error enum of the parsing. If no error has occured NoError is given.
    inline BacnetTagParserError error() {
        return _error;
    }

    inline bool hasError() {
        return _error != NoError;
    }

    inline bool hasNext() {
        return (_leftLength > 0);
    }

private:
    /**
      Retursn true if the LTV field is of value B'110'.
      \warning Assumes _actualPtr is pointing at initial octet.
      */

    /** Returns additional bytes consumed to parse the tag number
      \warning This function assumes that _actualPtr is pointing to the initial octet
      */
    quint8 decodeTagNumberHelper();

    //! Returns number of additional bytes needed to get the length. If the length is in LTV field, then 0 is returned.
    quint8 decodeDataLengthHelper(quint8 skipExtendedTagFields);

    /** Helper function, that checks if we can read more bytesNum from the buffer.
      If there is enough bytes to read, true is returned and _leftLength is reduced by bytesNum value. Otherwise, false is returned and error set to BufferOverrun.
      */
    bool reduceBytesLeft(quint16 bytesNum);

    //! Returns true if recently parsed tag is of requested data type or is context tag. Otherwise returns false and sets _error to AppTagNotRequestedType.
    bool checkCorrectAppOrCtxTagHelper(AppTags::BacnetTags dataType);

    bool checkCorrectLengthHelper(quint8 dataLength);

    bool valueLengthLessThanEqHelper(quint8 maxEqLength);
    bool valueLenthGreaterThanEqHelper(quint8 minEqLength);

private:
    //! \todo This function is redundant with decodeDataLengthHelper() one. Correct it!
    static qint16 decodeTagNumber_helper(quint8 *tagPtr, quint8 *tagNum, bool *isContextTag, quint16 leftLength);

private:
    quint8 *_trackedData;
    quint8 *_copiedData;

    /**this variable holds number of bytes that are left in the buffer.
      It gets decreased on invocation of
        # \sa decodeTagNumberHelper() by the number of bytes that were used to decode the tag number
        # \sa decodeDataLengthHelper() by the additional number of bytes that were used to store the length value. Notice that we don't take into account basic
        byte (tag octet) since it was included in a case above
        # \sa parseNext() - whenever we ivoke parse next, that means we shift along _valueLength bytes. Thus by this number the _leftLength has to be decreased.
        Each of these functions check for buffer overflow. If such issue occures, we don't parse anymore and set error to BufferOverrun. Same thing holds to all
        converstions to*() - if there is not ennough buffer left to carry out conversion, ok is set to false and _error to BufferOverrun.
      */
    quint16 _leftLength;

    quint8 *_actualTagPtr;
    bool _isContextTag;
    quint8 _tagNum;
    quint8 *_valuePtr;
    quint32 _valueLength;
    BacnetTagParserError _error;
};

}

#endif // BACNETTAGPARSER_H
