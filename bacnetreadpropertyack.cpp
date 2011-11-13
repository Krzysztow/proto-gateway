#include "bacnetreadpropertyack.h"

#include <QtCore>

#include "bacnetcoder.h"
#include "bacnettagparser.h"

using namespace Bacnet;

BacnetReadPropertyAck::BacnetReadPropertyAck():
        _data(0)
{

}

BacnetReadPropertyAck::BacnetReadPropertyAck(ReadPropertyServiceData &ackReadPrptyData, BacnetDataInterface *data):
        _readData(ackReadPrptyData),
        _data(data)
{

}

BacnetReadPropertyAck::~BacnetReadPropertyAck()
{
    //delete _data;//no need to delete it anymore - it's a SharedPointer.
}

qint32 BacnetReadPropertyAck::toRaw(quint8 *startPtr, quint16 buffLength)
{
    quint8 *actualPtr(startPtr);
    quint16 leftLength(buffLength);
    qint32 ret;

    //    //encode Object identifier
    //    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, _value.objId, true, 0);
    //    if (ret <= 0) {//something wrong?
    //        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode objId");
    //        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: objId : %d", ret);
    //        return ret;
    //    }
    //    actualPtr += ret;
    //    leftLength -= ret;
    //    //encode proeprty identifier
    //    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.propertyId, true, 1);
    //    if (ret <= 0) {//something wrong?
    //        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode propId");
    //        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
    //        return ret;
    //    }
    //    actualPtr += ret;
    //    leftLength -= ret;
    //    //encode array index if present - OPTIONAL
    //    if (ArrayIndexNotPresent != _value.arrayIndex) {//present
    //        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.arrayIndex, true, 2);
    //        if (ret <= 0) {//something wrong?
    //            Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode arrayIndex");
    //            qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
    //            return ret;
    //        }
    //        actualPtr += ret;
    //        leftLength -= ret;
    //    }

    ret = _readData.toRaw(actualPtr, leftLength);
    if (ret < 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode read property part");
        qDebug("BacnetReadPropertyAck::toRaw() : property encoding problem: read property : %d", ret);
        return ret;    }
    actualPtr += ret;
    leftLength -= ret;

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

    //! \note We can't use ReadPropertyServiceData::fromRaw() method here, since it checks for too many arguments. And we have PropertyValue following.

    //parse object identifier
    ret = bParser.parseNext();
    _readData.objId = bParser.toObjectId(&convOkOrCtxt);
    if (ret < 0 || !bParser.isContextTag(0))
        return -1;
    consumedBytes += ret;

    //parse property identifier
    ret = bParser.parseNext();
    _readData.propertyId = (BacnetProperty::Identifier)bParser.toUInt(&convOkOrCtxt);
    if (ret < 0 || !bParser.isContextTag(1))
        return -2;
    consumedBytes += ret;

    //parse OPTIONAL array index
    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if (2 == ret && convOkOrCtxt) {//there is an array index
        ret = bParser.parseNext();
        _readData.arrayIndex = bParser.toUInt(&convOkOrCtxt);
        if ( (ret < 0) || !convOkOrCtxt)
            return -3;
        consumedBytes += ret;
    } else {
        _readData.arrayIndex = ArrayIndexNotPresent;
    }

    //we are supposed to parse Abstract type, which type depends on the object type and property Id
    Q_ASSERT(_data.isNull());
    _data.clear();//just in case
    ret = BacnetTagParser::parseStructuredData(bParser, _readData.objId.objectType, _readData.propertyId,
                                               _readData.arrayIndex, 3, _data);


    if (ret <= 0) //something worng, check it
        return -4;
    consumedBytes += ret;

    return consumedBytes;
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

    const quint32 writeSize(64);
    quint8 writeRpAckData[writeSize];

    ret = rpAck.toRaw(&writeRpAckData[0], writeSize);
    Q_ASSERT(ret == rpAckLength);
    HelperCoder::printArray(writeRpAckData, ret, "Written ack:");

    return 0;
}
#endif
