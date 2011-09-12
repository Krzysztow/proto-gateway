#include "bacnetreadpropertyack.h"

#include <QtCore>

#include "bacnetdata.h"
#include "bacnetcoder.h"
#include "bacnettagparser.h"
#include "readpropertyservicedata.h"

BacnetReadPropertyAck::BacnetReadPropertyAck():
        _data(0)
{
}

BacnetReadPropertyAck::~BacnetReadPropertyAck()
{
    delete _data;
}

void BacnetReadPropertyAck::setData(Bacnet::BacnetDataInterface *data, Bacnet::ReadPropertyServiceData &ackReadPrptyData)
{
    Q_CHECK_PTR(data);
    Q_ASSERT(0 == _data);
    delete _data;//just in case - is it really needed?
    _data = data;
    _value = ackReadPrptyData;
}

qint32 BacnetReadPropertyAck::toRaw(quint8 *startPtr, quint16 buffLength)
{
    quint8 *actualPtr = startPtr;
    quint16 leftLength = buffLength;
    qint32 ret;

    //encode Object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, _value.objId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode objId");
        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: objId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode proeprty identifier
    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.propertyId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode propId");
        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode array index if present - OPTIONAL
    if (Bacnet::ArrayIndexNotPresent != _value.arrayIndex) {//present
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.arrayIndex, true, 2);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode arrayIndex");
            qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    //encode value
    ret = BacnetCoder::openingTagToRaw(actualPtr, leftLength, 3);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode opening tag");
        qDebug("BacnetReadPropertyAck::toRaw() : cannot encode opening tag: : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    Q_CHECK_PTR(_data);
    ret = _data->toRaw(actualPtr, leftLength);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode abstract data");
        qDebug("BacnetReadPropertyAck::toRaw() : cannot encode abstract data: : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    ret = BacnetCoder::closingTagToRaw(actualPtr, leftLength, 3);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode closing tag");
        qDebug("BacnetReadPropertyAck::toRaw() : cannot encode closing tag: : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    return actualPtr - startPtr;
}

qint32 BacnetReadPropertyAck::fromRaw(quint8 *startPtr, quint16 buffLength)
{
    BacnetTagParser bParser(startPtr, buffLength);

    qint16 ret;
    qint16 consumedBytes(0);
    bool convOkOrCtxt;

    //parse object identifier
    ret = bParser.parseNext();
    _value.objId = bParser.toObjectId(&convOkOrCtxt);
    if (ret < 0 || !bParser.isContextTag(0))
        return -1;
    consumedBytes += ret;

    //parse property identifier
    ret = bParser.parseNext();
    _value.propertyId = (BacnetProperty::Identifier)bParser.toUInt(&convOkOrCtxt);
    if (ret < 0 || !bParser.isContextTag(1))
        return -2;
    consumedBytes += ret;

    //parse OPTIONAL array index
    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if (2 == ret && convOkOrCtxt) {//there is an array index
        ret = bParser.parseNext();
        _value.arrayIndex = bParser.toUInt(&convOkOrCtxt);
        if (ret <0 | !convOkOrCtxt)
            return -3;
        consumedBytes += ret;
    } else {
        _value.arrayIndex = Bacnet::ArrayIndexNotPresent;
    }

    //we are supposed to parse Abstract type, which type depends on the object type and property Id
    Q_ASSERT(0 == _data);
    _data = 0;//just in case
    ret = BacnetTagParser::parseStructuredData(bParser, _value.objId.objectType, _value.propertyId,
                                               _value.arrayIndex, 3, &_data);


    if (ret <= 0) //something worng, check it
        return -4;
    consumedBytes += ret;

    return consumedBytes;
}

Bacnet::BacnetDataInterface *BacnetReadPropertyAck::data()
{
    return _data;
}

Bacnet::ReadPropertyServiceData &BacnetReadPropertyAck::value()
{
    return _value;
}

//#define BACNET_RP_ACK_TEST
#ifdef BACNET_RP_ACK_TEST
int main()
{
    quint8 rpAckData[] = {
        0x0c, 0x00, 0x00, 0x00, 0x05, 0x19, 0x55, 0x3e, 0x44, 0x42, 0x90, 0x99, 0x9a, 0x3f
    };
    const quint16 rpAckLength = sizeof(rpAckData);

    BacnetReadPropertyAck rpAck;
    quint8 ret =rpAck.fromRaw(rpAckData, rpAckLength);

    qDebug("Paresed %d bytes out of %d", ret, rpAckLength);
    Q_ASSERT(ret > 0);
    Q_ASSERT(rpAckLength == ret);

    return 0;
}
#endif
