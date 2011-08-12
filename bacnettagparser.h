#ifndef BACNETTAGPARSER_H
#define BACNETTAGPARSER_H

#include <QtCore>
#include <QBitArray>
#include "bacnetcoder.h"
#include "helpercoder.h"
#include "bacnetcommon.h"

//this is used for detection of open/close tag. They have to be context specific (Bit3 is set to 1) and
//110 when opening, whereas 111 when closing tag is encoded
#define OPEN_CLOSE_TAG_MASK (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2 | BitFields::Bit3)
#define OPEN_TAG (0x00 | BitFields::Bit1 | BitFields::Bit2 | BitFields::Bit3)
#define CLOSE_TAG (0x00 | BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2 | BitFields::Bit3)

void printArray(quint8 *ptr, int size, char *pretext);
QDebug operator<<(QDebug dbg, const QBitArray& z);
void printBin(int value, int lsbBitsNum, const char *prestring, const char *poststring);

class BacnetTagParser
{
public:
    enum BacnetAbortReason {
        AbortReasonOther                        = 0,
        AbortReasonBufferOverflow               = 1,
        AbortReasonInvalidApduInThisState 	= 2,
        AbortReasonPreemptedByHigherPriorityTask= 3,
        AbortReasonSegmentationNotSupported 	= 4
                                              };

    enum BacnetRejectReason {
        RejectReasonOther                       = 0,
        RejectReasonBufferOverflow              = 1,
        RejectReasonInconsistentParameters 	= 2,
        RejectReasonInvalidParameterDataType 	= 3,
        RejectReasonInvalidTag                  = 4,
        RejectReasonMissingRequiredParameter 	= 5,
        RejectReasonParameterOutOfRange 	= 6,
        RejectReasonTooManyArguments            = 7,
        RejectReasonUndefinedEnumeration 	= 8,
        RejectReasonUnrecognizedService 	= 9
                                              };

    enum BacnetErrorClass {
        ClassDevice 	= 0,
        ClassObject 	= 1,
        ClassProperty 	= 2,
        ClassResources 	= 3,
        ClassSecurity 	= 4,
        ClassServices 	= 5,
        ClassVt 	= 6
                      };

    enum BacnetErrorCode {
        CodeOther                                   	= 0,
        CodeAuthenticationFailed                   	= 1,
        CodeCharacterSetNotSupported             	= 41,
        CodeConfigurationInProgress               	= 2,
        CodeDatatypeNotSupported                  	= 47,
        CodeDeviceBusy                             	= 3,
        CodeDuplicateName                          	= 48,
        CodeDplicateObjectId                      	= 49,
        CodeDynamicCreationNotSupported          	= 4,
        CodeFileAccessDenied                      	= 5,
        CodeIncompatibleSecurityLevels            	= 6,
        CodeInconsistentParameters                 	= 7,
        CodeInconsistentSelectionCriterion        	= 8,
        CodeInvalidArrayIndex                     	= 42,
        CodeInvalidConfigurationData              	= 46,
        CodeInvalidDataType                       	= 9,
        CodeInvalidFileAccessMethod              	= 10,
        CodeInvalidFileStartPosition             	= 11,
        CodeInvalidOperatorName                   	= 12,
        CodeInvalidParameterDataType             	= 13,
        CodeInvalidTimeStamp                      	= 14,
        CodeKeyGenerationError                    	= 15,
        CodeMissingRequiredParameter              	= 16,
        CodeNoObjectsOfSpecifiedType            	= 17,
        CodeNoSpaceForObject                     	= 18,
        CodeNoSpaceToAddListElement                     = 19,
        CodeNoSpaceToWriteProperty              	= 20,
        CodeNoVtSessionsAvailable                	= 21,
        CodeObjectDeletionNotPermitted           	= 23,
        CodeObjectIdentifierAlreadyExists        	= 24,
        CodeOperationalProblem                     	= 25,
        CodeOptionalFunctionalityNotSupported   	= 45,
        CodePasswordFailure                        	= 26,
        CodePropertyIsNotAList                  	= 22,
        CodePropertyIsNotAnArray                	= 50,
        CodeReadAccessDenied                      	= 27,
        CodeSecurityNotSupported                  	= 28,
        CodeServiceRequestDenied                  	= 29,
        CodeTimeout                                 	= 30,
        CodeUnknownObject                               = 31,
        CodeUnknownProperty                             = 32,
        // this enumeration was removed                 	= 33,
        CodeUnknownVtClass                              = 34,
        CodeUnknownVtSession                            = 35,
        CodeUnsupportedObjectType                       = 36,
        CodeValueOutOfRange                             = 37,
        CodeVtSessionAlreadyClosed                      = 38,
        CodeVtSessionTerminationFailure                 = 39,
        CodeWriteAccessDenied                           = 40,
        // see characterSetNotSupported              	= 41,
        // see invalidArrayIndex                      	= 42,
        CodeCovSubscriptionFailed                       = 43,
        CodeNotCovProperty                              = 44,
        // see optionalFunctionalityNotSupported,    	= 45,
        // see invalidConfigurationData               	= 46,
        // see datatypeNotSupported                   	= 47,
        // see duplicateName                           	= 48,
        // see duplicateObjectId                      	= 49,
        // see propertyIsNotAnArray                 	= 50
    };

    enum BacnetTagParserError {
        NoError = 0,

        BufferOverrun = -1
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

    //default copy constructor is ok, when we need to have our own copy of data, call copyData()

    ~BacnetTagParser()
    {
        delete []_copiedData;
    }

    /**
      Sets the data to be parsed.
      \note Keep in mind that the buffer data cannot be altered while parsing. To be able to do so, use \sa copyData.
      */
    void setData(quint8 *data, quint16 length)
    {
        if (0 != _copiedData) {//shouldn't be a performance bottleneck, since we wouldn't use it much often, I suppose
            delete []_copiedData;
            _copiedData = 0;
        }

        _leftLength = length;
        _trackedData = data;
        _actualTagPtr = data;
        _valuePtr = _actualTagPtr;
        _valueLength = 0;
        _error = NoError;
    }

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

    inline qint16 tagNumber()
    {
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

    BacnetCommon::ObjectId toObjectId(bool *ok = 0);

    /**
      Returns QString with decoded value from token value we are currently at (if the token is string).
      \not that currently, ANSI, UCS-2, UCS-4 and ISO 8859-1 is supported (no JIS C 6226 and DBCS)
      */
    QString toString(bool *ok = 0);

    //! Returns integer value representing enumeration (if the current token is enumerated)
    quint32 toEumerated(bool *ok = 0);

    inline bool isOpeningTag() {
        //make sure that if it's an opening or closing tag, the class field is set to context
        Q_ASSERT(((*_actualTagPtr & (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2)) >= 0x0e) ? BacnetCoder::isContextTag(_actualTagPtr) : true);
        return (OPEN_TAG == (*_actualTagPtr & OPEN_CLOSE_TAG_MASK));
    }

    inline bool isClosingTag() {
        //make sure that if it's an opening or closing tag, the class field is set to context
        Q_ASSERT(((*_actualTagPtr & (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2)) >= 0x0e) ? BacnetCoder::isContextTag(_actualTagPtr) : true);
        return (CLOSE_TAG == (*_actualTagPtr & OPEN_CLOSE_TAG_MASK));
    }

    /**
      Returns true if the initial tag LTV is of value B'110' or B'111'.
      \warning assumes _actualPtr points at initial octet.
      */
    inline bool isOpeningOrClosingTag() {
        //make sure that if it's an opening or closing tag, the class field is set to context
        Q_ASSERT(((*_actualTagPtr & (BitFields::Bit0 | BitFields::Bit1 | BitFields::Bit2)) >= 0x0e) ? BacnetCoder::isContextTag(_actualTagPtr) : true);
        return ((*_actualTagPtr & OPEN_CLOSE_TAG_MASK) >= 0x0e);
    }

    quint16 valueLength();
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
    bool reduceLeftBytes(quint16 bytesNum);


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




#endif // BACNETTAGPARSER_H
