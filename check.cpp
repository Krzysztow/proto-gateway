#include "bacnettagparser.h"

#define TAG_FIELD_MASK 0xf0
#define LENGTH_FIELD_MASK 0x07
#define EXT_LENGTH_VALUE 0x05

qint16 BacnetTagParser::parseNext()
{
    _actualTagPtr = _valuePtr + _valueLength;//move tag pointer to the next tag
    //_actual pointer has to be pointing at the initial octet of the tag to be decoded!
    quint8 fieldsShift(0);//shift of tag, then length, then value fields - this shift could result due to tag>=16, or length > 5
    fieldsShift = decodeTagNumberHelper();

    if (!isContextTag() && (AppTags::Boolean == _tagNum)) {
        _valueLength = 1;
        _valuePtr = _actualTagPtr;
    } else {
        fieldsShift += decodeDataLengthHelper(fieldsShift);
        _valuePtr = _actualTagPtr + fieldsShift + 1;//1 is that normally the value field comes straight after the tag
    }

    return fieldsShift + _valueLength;
}

quint8 BacnetTagParser::decodeTagNumberHelper()
{
    _isContextTag = BacnetCoder::isContextTag(_actualTagPtr);
    _tagNum = (*_actualTagPtr & TAG_FIELD_MASK) >> 4;
    if (AppTags::ExtendedTagNumber == _tagNum) {
        Q_ASSERT(_isContextTag);
        _tagNum = *(_actualTagPtr + 1);
        return 1;
    }

    return 0;
}

quint8 BacnetTagParser::decodeDataLengthHelper(quint8 skipExtendedTagFields)
{
    if (isOpeningOrClosingTag()) {//there is no value carried - move pointer after tag and set length to 0
        _valueLength = 0;
    } else {
        _valueLength = (*_actualTagPtr) & LENGTH_FIELD_MASK;
        if (EXT_LENGTH_VALUE == _valueLength) {//if is equal to B'101', then the length is larger than 4
            quint8 *extLengthPtr = _actualTagPtr + skipExtendedTagFields + 1;
            if (*extLengthPtr < 254) {//if less, then the length is within 5-253
                _valueLength = *extLengthPtr;
                return 1;
            } else if (254 == *extLengthPtr) {//the length takes 16bits
                quint16 tempLenght;
                quint8 ret = HelperCoder::uint16FromRaw(extLengthPtr, &tempLenght);
                _valueLength = tempLenght;
                Q_ASSERT(ret > 0);
                return ret;
            } else {//it takes 32 bits
                quint8 ret = HelperCoder::uint32FromRaw(extLengthPtr, &_valueLength);
                Q_ASSERT(ret > 0);
                return ret;
            }
        } else {
            if (isOpeningOrClosingTag()) {
                _valueLength = 0;
            }
        }
    }
    return 0;
}

QByteArray BacnetTagParser::toByteArray(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    QByteArray ret;
    if (_isContextTag || (AppTags::OctetString == tagNumber())) {
        if (ok) *ok = true;
        //! \todo maybe we should use QByteArray::setRawData() instead of fromRawData - no copying will occure.
        ret = QByteArray::fromRawData((const char*)_valuePtr, _valueLength);
        return ret;
    }

    if (ok) *ok = false;
    return ret;
}

QBitArray BacnetTagParser::toBitArray(bool *ok)
{
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(_valueLength >= 2);//has to have the free bits number field and at least one bits-field
    QBitArray ret;
    if ( (_isContextTag || (AppTags::BitString == _tagNum)) && (_valueLength >= 2)) {
        if (ok) *ok = true;
        quint16 numOfBits = 8*(_valueLength-1) - *_valuePtr;
        ret.resize(numOfBits);
        for (int i = numOfBits-1; i >=0; --i) {
            ret.setBit(numOfBits-i-1, (_valuePtr+1)[i/8] & (128>>(i%8)));
        }
        return ret;
    }

    if (ok) *ok = false;
    return ret;
}

QString BacnetTagParser::toString(bool *ok)
{
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(_valueLength >= 2);//at least lenght and one string byte has to be encoded
    QString ret;
    if ( (_isContextTag || (AppTags::CharacterString == _tagNum)) && (_valueLength >= 2)) {
        BacnetCoder::CharacterSet encoding = (BacnetCoder::CharacterSet)*_valuePtr;
        if (ok) *ok = true;
        switch (encoding)
        {
        case (BacnetCoder::AnsiX3_4): {
                ret = QString::fromAscii((char *)(_valuePtr + 1), _valueLength - 1);
                break;
            }
        case (BacnetCoder::IbmDbcs): {
                //                break;//fall through
            }
        case (BacnetCoder::JisC6266): {
                qWarning("This encoding is not supported!");
                //                Q_ASSERT(false);
                if (ok) *ok = false;
                break;
            }
        case (BacnetCoder::UCS_4): {
                Q_ASSERT((_valueLength-1)%sizeof(quint32) == 0);
                quint16 charNum = (_valueLength - 1)/sizeof(quint32);
                quint32 *actualLetter = (quint32*)(_valuePtr + 1);
                ret.reserve(charNum);
                quint32 letterValue;
                for (int i =0; i<charNum; ++i) {
                    HelperCoder::uint32FromRaw((quint8*)(actualLetter), &letterValue);
                    ret += QString::fromUcs4((quint32*)&letterValue, 1);
                    ++actualLetter;
                }
                break;
            }
        case (BacnetCoder::UCS_2): {
                Q_ASSERT((_valueLength-1)%sizeof(ushort) == 0);
                quint16 charNum = (_valueLength - 1)/sizeof(ushort);
                ushort *actualLetter = (ushort*)(_valuePtr + 1);
                ret.reserve(charNum);
                quint16 letterValue;
                for (int i =0; i<charNum; ++i) {
                    HelperCoder::uint16FromRaw((quint8*)(actualLetter), &letterValue);
                    ret += QString::fromUtf16((ushort*)&letterValue, 1);
                    ++actualLetter;
                }
                break;
            }
        case (BacnetCoder::ISO_8859_1): {
                ret = QString::fromLatin1((char*)(_valuePtr + 1), _valueLength -1);
                break;
            }
        default:
            if (ok) *ok = false;
        }
        return ret;
    }

    if (ok) *ok = false;
    return ret;
}

void BacnetTagParser::copyData()
{
    if (_copiedData == _trackedData)//already copied
        return;

    if (0 != _copiedData)
        delete []_copiedData;

#warning "Change limit to maximum APDU size"
    Q_ASSERT(_length <= 1024);

    _copiedData = new quint8[_length];
    memcpy(_copiedData, _trackedData, _length);

    if ( (_actualTagPtr >= _trackedData) || (_actualTagPtr <= _trackedData+_length) ) {
        _actualTagPtr = _copiedData + (_actualTagPtr - _trackedData);//move ptr of actual tag to the copied buffer
        _valuePtr = _copiedData + (_valuePtr - _trackedData);//move ptr of actual value data to the copied buffer
    }

    _trackedData = _copiedData;//from now on, we act on the copied buffer
}

bool BacnetTagParser::toBoolean(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(1 == _valueLength);
    if ( (_isContextTag || (BacnetCoder::Boolean == _tagNum)) && (1 == _valueLength))  {
        if (ok) *ok = true;
        return (bool)(*_valuePtr & 0x01);
    }
    if (ok) *ok = false;
    return false;
}

quint32 BacnetTagParser::toUInt(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(_valueLength <= 4);
    quint32 ret;
    if ( (_isContextTag || (AppTags::UnsignedInteger == _tagNum)) && (_valueLength <= 4)) {
        if (ok) *ok = true;
        HelperCoder::uint32fromVarLengthRaw(_valuePtr, &ret, _valueLength);
        return ret;
    }
    if (ok) *ok = false;
    return 0;
}


qint32 BacnetTagParser::toInt(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(_valueLength <= 4);
    qint32 ret;
    if ( (_isContextTag || (AppTags::SignedInteger == _tagNum)) && (_valueLength <= 4)) {
        if (ok) *ok = true;
        HelperCoder::sint32fromVarLengthRaw(_valuePtr, &ret, _valueLength);
        return ret;
    }

    if (ok) *ok = false;
    return 0;
}

quint32 BacnetTagParser::toEumerated(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(_valueLength <= 4);
    quint32 ret;
    if ( (_isContextTag || (AppTags::Enumerated == _tagNum)) && (_valueLength <= 4)) {
        if (ok) *ok = true;
        HelperCoder::uint32fromVarLengthRaw(_valuePtr, &ret, _valueLength);
        return ret;
    }

    if (ok) *ok = false;
    return 0;
}


float BacnetTagParser::toFloat(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(4 == _valueLength);
    float ret;
    if ( (_isContextTag || (AppTags::Real == tagNumber())) && (4 == _valueLength)) {
        if (ok) *ok = true;
        HelperCoder::floatFromRaw(_valuePtr, &ret);
        return ret;
    }

    if (ok) *ok = true;
    return ret;
}

double BacnetTagParser::toDouble(bool *ok) {
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(8 == _valueLength);
    double ret;
    if ( (_isContextTag || (AppTags::Double == tagNumber())) && (8 == _valueLength)) {
        if (ok) *ok = true;
        HelperCoder::doubleFromRaw(_valuePtr, &ret);
        return ret;
    }

    if (ok) *ok = false;
    return 0;
}

QDate BacnetTagParser::toDate(bool *ok)
{
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(4 == _valueLength);
    QDate ret;
    if ( (_isContextTag || (AppTags::Date == tagNumber())) && (4 == _valueLength)) {
        if (ok) *ok = true;
        quint16 year = *_valuePtr + 1900;
        quint8 month = *(_valuePtr +  1);
        quint8 dayOfMonth = *(_valuePtr + 2);
        quint8 dayOfWeek = *(_valuePtr + 3);
        Q_UNUSED(dayOfWeek);
        //! \todo what with the unspecified fields? 0xff
        ret.setYMD(year, month, dayOfMonth);
        return ret;
    }

    if (ok) *ok = false;
    return ret;
}

QTime BacnetTagParser::toTime(bool *ok)
{
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(4 == _valueLength);
    QTime ret;
    if ( (_isContextTag || (AppTags::Time == tagNumber())) && (4 == _valueLength)) {
        if (ok) *ok = true;
        quint8 hour = *_valuePtr;
        quint8 minute = *(_valuePtr +  1);
        quint8 seconds = *(_valuePtr + 2);
        quint8 hundredsSecs = *(_valuePtr + 3);
        //! \todo what with the unspecified fields? 0xff
        ret.setHMS(hour, minute, seconds, 10 * hundredsSecs);
        return ret;
    }

    if (ok) *ok = false;
    return ret;
}

Bacnet::ObjectIdStruct BacnetTagParser::toObjectId(bool *ok)
{
    Q_CHECK_PTR(_valuePtr);
    Q_ASSERT(4 == _valueLength);
    //here we return by value, maybe should change it?
    Bacnet::ObjectIdStruct ret = {BacnetObjectType::Undefined, 0};
    if ( (_isContextTag || (AppTags::BacnetObjectIdentifier == tagNumber())) && (4 == _valueLength)) {
        quint16 objType = *(quint16*)_valuePtr;
        objType = qFromBigEndian(objType);
        objType >>= 6;//get rid of the part from the instance number
        quint32 instNum = qFromBigEndian(*(quint32*)_valuePtr);
        instNum &= 0x3fffff;//get rid of the part of object type
        ret.objectType = (BacnetObjectType::ObjectType)objType;
        ret.instanceNum = instNum;
        if (ok) *ok = true;
        return ret;
    }

    if (ok) *ok = false;
    return ret;
}

//Application data parsing checker!
#define TEST_CONVERSIONS
#ifdef TEST_CONVERSIONS
int main(int argc, char *argv[])
{
    //check NULL
    quint8 dataNull[] = {0x00};
    BacnetTagParser bsp(dataNull, 1);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::Null);

    bool convOk;

    //check Boolean application class
    quint8 dataBoolApp[] = {0x10};
    bsp.setData(dataBoolApp, 1);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp._valueLength == 1);
    Q_ASSERT(bsp.tagNumber() == AppTags::Boolean);
    Q_ASSERT(bsp.toBoolean(&convOk) == false);
    Q_ASSERT(convOk);

    //check Boolean context tag
    quint8 dataBoolCtxt[] = {0x29, 0x01};
    bsp.setData(dataBoolCtxt, 2);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == true);
    Q_ASSERT(bsp.tagNumber() == 2);
    Q_ASSERT(bsp.toBoolean(&convOk) == true);
    Q_ASSERT(convOk);

    //check two consecutive
    quint8 dataBoolCons[] = {0x10, 0x29, 0x01};
    bsp.setData(dataBoolCons, 3);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp._valueLength == 1);
    Q_ASSERT(bsp.tagNumber() == AppTags::Boolean);
    Q_ASSERT(bsp.toBoolean(&convOk) == false);
    Q_ASSERT(convOk);
    bsp.parseNext();
    //check Boolean context tag
    Q_ASSERT(bsp.isContextTag() == true);
    Q_ASSERT(bsp.tagNumber() == 2);
    Q_ASSERT(bsp.toBoolean(&convOk) == true);
    Q_ASSERT(convOk);

    //check Signed Integer Value
    quint8 dataSigned[]= {0x31, 0x48};
    bsp.setData(dataSigned, 2);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::SignedInteger);
    Q_ASSERT(bsp.toInt(&convOk) == 72);
    Q_ASSERT(convOk);

    //check Real value
    quint8 dataReal[] = {0x44, 0x42, 0x90, 0x00, 0x00};
    bsp.setData(dataReal, 5);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::Real);
    Q_ASSERT( (bsp.toFloat(&convOk) - 72.0) < 0.01 );
    Q_ASSERT(convOk);

    //check Double
    quint8 dataDouble[] = {0x55, 0x08, 0x40, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bsp.setData(dataDouble, 10);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::Double);
    Q_ASSERT( (bsp.toDouble(&convOk) - 72.0) < 0x0001 );
    Q_ASSERT(convOk);

    //check Octet String
    quint8 dataOctetStr[] = {0x63, 0x12, 0x34, 0xff};
    bsp.setData(dataOctetStr, 4);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::OctetString);
    QByteArray octetRes = bsp.toByteArray(&convOk);
    Q_ASSERT(convOk);
    qDebug()<<"Got octet string"<<octetRes.toHex();

    quint8 ansiString[] = {0x75, 0x19, 0x00,
                           0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x42, 0x41,
                           0x43, 0x6E, 0x65, 0x74, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x21};
    bsp.setData(ansiString, sizeof ansiString);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::CharacterString);
    qDebug()<<"ANSI"<<bsp.toString(&convOk);
    Q_ASSERT(convOk);


    char dbcsString[] = {0x75, 0x31,
                         0x01, 0x03, 0x52, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x42, 0x41,
                         0x43, 0x6E, 0x65, 0x74, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x21};
    bsp.setData((quint8*)dbcsString, sizeof dbcsString);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::CharacterString);
    qDebug()<<"Got from dbcs"<<bsp.toString(&convOk);
    Q_ASSERT(!convOk);//we know it's unsupported

    char ucs2String[] = {0x75, 0x31,
                         0x04, 0x00, 0x54, 0x00, 0x68, 0x00, 0x69, 0x00,
                         0x73, 0x00, 0x20, 0x00, 0x69, 0x00, 0x73, 0x00,
                         0x20, 0x00, 0x61, 0x00, 0x20, 0x00, 0x42, 0x00,
                         0x41, 0x00, 0x43, 0x00, 0x6E, 0x00, 0x65, 0x00,
                         0x74, 0x00, 0x20, 0x00, 0x73, 0x00, 0x74, 0x00,
                         0x72, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00,
                         0x21};
    bsp.setData((quint8*)ucs2String, sizeof ucs2String);
    bsp.parseNext();
    qDebug()<<"Got from ucs2"<<bsp.toString(&convOk);
    Q_ASSERT(convOk);
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::CharacterString);

    //UCS2 is UTF16
    //UCS4 is UTF32
    //ISO 8859-1 = Latin-1
    //ASCII is the American Standard Code for Information Interchange, also known as ANSI X3.4.

    quint8 bitString[] = {0x82, 0x03, 0xa8};
    bsp.setData(bitString, sizeof bitString);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::BitString);
    QBitArray bArray = bsp.toBitArray(&convOk);
    Q_ASSERT(convOk);
    qDebug()<<"Resulting bit array"<<bArray<<bArray[4];

    //chek enumerated
    quint8 enumData[] =  {0x91, 0x00};
    bsp.setData(enumData, sizeof enumData);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::Enumerated);
    Q_ASSERT(bsp.toEumerated(&convOk) == 0x00);
    Q_ASSERT(convOk);

    //check date
    quint8 dateData[] = {0xa4, 0x5b, 0x01, 0x18, 0x04};
    bsp.setData(dateData, sizeof dateData);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::Date);
    qDebug()<<"Date: "<<bsp.toDate(&convOk);
    Q_ASSERT(convOk);

    //check Time
    quint8 timeDatap[] = {0xb4, 0x11, 0x23, 0x2d, 0x11};
    bsp.setData(timeDatap, sizeof timeDatap);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::Time);
    qDebug()<<"Time"<<bsp.toTime(&convOk);
    Q_ASSERT(convOk);

    //check object identifier
    quint8 objIdData[] = {0xc4, 0x00, 0xc0, 0x00, 0x0f};
    bsp.setData(objIdData, sizeof objIdData);
    bsp.parseNext();
    Q_ASSERT(bsp.isContextTag() == false);
    Q_ASSERT(bsp.tagNumber() == AppTags::BacnetObjectIdentifier);
    Bacnet::ObjectIdStruct objId = bsp.toObjectId(&convOk);
    Q_ASSERT(convOk);
    Q_ASSERT(BacnetCommon::ObjectTypeBinaryInput == objId.objectType);
    Q_ASSERT(15 == objId.instanceNum);
    qDebug()<<"Object type returned"<<objId.instanceNum<<objId.objectType;

    //CHECKING CONTEXT TAGS
    {
        quint8 nullCtxt[] = {0x38};
        bsp.setData(nullCtxt, sizeof nullCtxt);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 3);
        Q_ASSERT(bsp._valueLength == 0);
    }

    {
        quint8 boolCtxt[] = {0x69, 0x00};
        bsp.setData(boolCtxt, sizeof boolCtxt);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 6);
        Q_ASSERT(bsp._valueLength == 1);
        Q_ASSERT(bsp.toBoolean(&convOk) == false);
        Q_ASSERT(convOk);
    }

    {
        quint8 intCtxt[] = {0x0a, 0x01, 0x00};
        bsp.setData(intCtxt, sizeof intCtxt);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 0);
        Q_ASSERT(bsp.toUInt(&convOk) == 256);
        Q_ASSERT(convOk);
    }

    {
        quint8 sintCtxt[] = {0x59, 0xb8};
        bsp.setData(sintCtxt, sizeof sintCtxt);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 5);
        //        qint32 result = bsp.toInt();
        //        printBin(result, 32, "Binary result:", "");
        //        qDebug()<<"Result"<<result;
        Q_ASSERT(bsp.toInt(&convOk) == -72);
        Q_ASSERT(convOk);
    }

    {
        quint8 realCtx[] = {0x0c, 0xc2, 0x05, 0x33, 0x33};
        bsp.setData(realCtx, sizeof realCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 0);
        Q_ASSERT(bsp.toFloat(&convOk) - -33.3 < 0.01);
        Q_ASSERT(convOk);
    }

    {
        quint8 doubleCtx[] = {0x1d, 0x08,
                              0xC0, 0x40, 0xA6, 0x66, 0x66, 0x66, 0x66, 0x66};
        bsp.setData(doubleCtx, sizeof doubleCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 1);
        Q_ASSERT(bsp.toDouble(&convOk) - -33.3 < 0.0001);
        Q_ASSERT(convOk);
    }

    {
        quint8 octetStrCtx[] = {0x1a,
                                0x43, 0x21};
        bsp.setData(octetStrCtx, sizeof octetStrCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 1);
        qDebug()<<"Octet string"<<bsp.toByteArray(&convOk).toHex();
        Q_ASSERT(convOk);
    }

    {
        quint8 charStrCtx[] = {0x5d, 0x19,
                               0x00,
                               0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x42, 0x41,
                               0x43, 0x6E, 0x65, 0x74, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x21};
        bsp.setData(charStrCtx, sizeof charStrCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 5);
        Q_ASSERT(convOk);
    }

    {
        quint8 bitStrCtx[] = {0x0a, 0x03, 0xa8};
        bsp.setData(bitStrCtx, sizeof bitStrCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 0);
        QBitArray checkArray; checkArray.resize(5);
        checkArray.setBit(0, true);
        checkArray.setBit(2, true);
        checkArray.setBit(4, true);
        Q_ASSERT(bsp.toBitArray(&convOk) == checkArray);
        Q_ASSERT(convOk);
    }

    {
        quint8 enumCtx[] = {0x99, 0x00};
        bsp.setData(enumCtx, sizeof enumCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 9);
        Q_ASSERT(bsp.toEumerated(&convOk) == BacnetCommon::ObjectTypeAnalogInput);
        Q_ASSERT(convOk);
    }

    {
        quint8 dateCtx[] = {0x9c,
                            0x5b, 0x01, 0x18, 0x05};
        bsp.setData(dateCtx, sizeof dateCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 9);
        QDate date = bsp.toDate(&convOk);
        Q_ASSERT(date.year() == 1991 && date.month() == 1 && date.day() == 24);
        Q_ASSERT(convOk);
    }

    {
        quint8 timeCtx[] = {0x4c,
                            0x11, 0x23, 0x2d, 0x11};
        bsp.setData(timeCtx, sizeof timeCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 4);
        QTime time = bsp.toTime(&convOk);
        Q_ASSERT(time.hour() == 17 && time.minute() == 35 && time.second() == 45);
        Q_ASSERT(convOk);
    }

    {
        quint8 identifierCtx[] = {0x4c,
                                  0x00, 0xc0, 0x00, 0x0f};
        bsp.setData(identifierCtx, sizeof identifierCtx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.tagNumber() == 4);
        Bacnet::ObjectIdStruct identifier = bsp.toObjectId(&convOk);
        Q_ASSERT(convOk);
        Q_ASSERT(identifier.instanceNum == 15 && identifier.objectType == BacnetCommon::ObjectTypeBinaryInput);
    }

    //Application SEQUENCE
    //    ASN.1 = BACnetDateTime
    //    Value = January 24, 1991, 5:35:45.17 P.M.
    //    Application Tag = Date (Tag Number = 10)
    //    Encoded Tag = X'A4'
    //    Encoded Data = X'5B011805'
    //    Application Tag = Time (Tag Number = 11)
    {
        quint8 seq1Ctx[] = {0xa4,
                            0x5B, 0x01, 0x18, 0x05,
                            0xb4,
                            0x11, 0x23, 0x2D, 0x11};
        bsp.setData(seq1Ctx, sizeof seq1Ctx);
        bsp.parseNext();
        Q_ASSERT(bsp.tagNumber() == AppTags::Date);
        qDebug()<<"Seq 1"<<bsp.toDate(&convOk);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.tagNumber() == AppTags::Time);
        qDebug()<<"Seq 2"<<bsp.toTime(&convOk);
        Q_ASSERT(convOk);
    }

    //CONTEXT TAGGED SEQUENCE
    {
        quint8 seq2Ctx[] = {0x0e,
                            0xa4,
                            0x5b, 0x01, 0x18, 0x05,
                            0xb4,
                            0x11, 0x23, 0x2d, 0x11,
                            0x0f
                        };
        bsp.setData(seq2Ctx, sizeof seq2Ctx);
        bsp.parseNext();
        Q_ASSERT(bsp.isOpeningTag());
        Q_ASSERT(bsp.tagNumber() == 0);
        bsp.parseNext();
        Q_ASSERT(bsp.tagNumber() == AppTags::Date);
//        qDebug()<<"seq 3"<<bsp.toDate(&convOk);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.tagNumber() == AppTags::Time);
//        qDebug()<<"Seq 4"<<bsp.toTime(&convOk);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.isClosingTag());
    }

    //Sequence of primitive data
    {
        quint8 seq1Ctx[] = {0xa4,
                            0x5B, 0x01, 0x18, 0x05,
                            0xb4,
                            0x11, 0x23, 0x2D, 0x11};
        bsp.setData(seq1Ctx, sizeof seq1Ctx);
        bsp.parseNext();
        Q_ASSERT(bsp.tagNumber() == AppTags::Date);
//        qDebug()<<"Seq 1"<<bsp.toDate(&convOk);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.tagNumber() == AppTags::Time);
//        qDebug()<<"Seq 2"<<bsp.toTime(&convOk);
        Q_ASSERT(convOk);
    }

    //APP TAGGED SEQUENCE
    {
        quint8 seq3App[] = {0x21, 0x01,
                            0x21, 0x02,
                            0x21, 0x04};
        bsp.setData(seq3App, sizeof seq3App);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == false);
        Q_ASSERT(bsp.tagNumber() == AppTags::UnsignedInteger);
        Q_ASSERT(bsp.toUInt(&convOk) == 1);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == false);
        Q_ASSERT(bsp.tagNumber() == AppTags::UnsignedInteger);
        Q_ASSERT(bsp.toUInt(&convOk) == 2);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == false);
        Q_ASSERT(bsp.tagNumber() == AppTags::UnsignedInteger);
        Q_ASSERT(bsp.toUInt(&convOk) == 4);
        Q_ASSERT(convOk);
    }

    //CONTEXT TAGGED SEQUENCE OF primitive data
    {
        quint8 seq4Ctx[] = {0x1e,
                            0x021, 0x01,
                            0x21, 0x02,
                            0x021, 0x04,
                            0x1f};
        bsp.setData(seq4Ctx, sizeof seq4Ctx);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.isOpeningOrClosingTag());
        Q_ASSERT(bsp.isOpeningTag());
        Q_ASSERT(bsp.isClosingTag() == false);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == false);
        Q_ASSERT(bsp.tagNumber() == AppTags::UnsignedInteger);
        Q_ASSERT(bsp.toUInt(&convOk) == 1);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == false);
        Q_ASSERT(bsp.tagNumber() == AppTags::UnsignedInteger);
        Q_ASSERT(bsp.toUInt(&convOk) == 2);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == false);
        Q_ASSERT(bsp.tagNumber() == AppTags::UnsignedInteger);
        Q_ASSERT(bsp.toUInt(&convOk) == 4);
        Q_ASSERT(convOk);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() == true);
        Q_ASSERT(bsp.isOpeningOrClosingTag());
        Q_ASSERT(bsp.isOpeningTag() == false);
        Q_ASSERT(bsp.isClosingTag());
    }

    //APP TAGGED SEQUENCE OF constructed data
    {
        quint8 seq4CConstr[] = {0xa4,
                            0x5B, 0x01, 0x18, 0x04,
                            0xb4,
                            0x11, 0x00, 0x00, 0x00,
                            0xa4,
                            0x5B, 0x01, 0x18, 0x04,
                            0xb4,
                            0x12, 0x2D, 0x00, 0x00};
        bsp.setData(seq4CConstr, sizeof seq4CConstr);
        {//first ocnstructed data
            bsp.parseNext();
            Q_ASSERT(!bsp.isContextTag() && bsp.tagNumber() == AppTags::Date);
            Q_ASSERT(bsp.toDate(&convOk) == QDate(1991, 1, 24));
            Q_ASSERT(convOk);
            bsp.parseNext();
            Q_ASSERT(!bsp.isContextTag() && bsp.tagNumber() == AppTags::Time);
            Q_ASSERT(bsp.toTime(&convOk) == QTime(17, 00, 0, 0));
            Q_ASSERT(convOk);

        }
        {//second ocnstructed data
            bsp.parseNext();
            Q_ASSERT(!bsp.isContextTag() && bsp.tagNumber() == AppTags::Date);
            Q_ASSERT(bsp.toDate(&convOk) == QDate(1991, 1, 24));
            Q_ASSERT(convOk);
            bsp.parseNext();
            Q_ASSERT(!bsp.isContextTag() && bsp.tagNumber() == AppTags::Time);
            Q_ASSERT(bsp.toTime(&convOk) == QTime(18, 45, 0, 0));
            Q_ASSERT(convOk);
        }
    }

    //ENCODING of choice value, primitive
    {
        quint8 choicePrim[] = {0x0c,
                               0x11, 0x23, 0x2D, 0x11};
        bsp.setData(choicePrim, sizeof choicePrim);
        bsp.parseNext();
        Q_ASSERT(bsp.isContextTag() && bsp.tagNumber() == 0);
        Q_ASSERT(bsp.toTime(&convOk) == QTime(17, 35, 45, 170));
        Q_ASSERT(convOk);
    }


    return 0;
}
#endif //TEST_CONVERSIONS

