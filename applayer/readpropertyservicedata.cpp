#include "readpropertyservicedata.h"

#include "bacnetcoder.h"
#include "bacnettagparser.h"

using namespace Bacnet;

ReadPropertyServiceData::ReadPropertyServiceData(ObjectIdStruct objId, BacnetPropertyNS::Identifier propertyId,
                          quint32 arrayIndex)
{
    this->objId = objId;
    this->propertyId = propertyId;
    this->arrayIndex = arrayIndex;
}

ReadPropertyServiceData::ReadPropertyServiceData()
{
}

qint32 ReadPropertyServiceData::fromRaw(quint8 *serviceData, quint16 bufferLength)
{
    BacnetTagParser bParser(serviceData, bufferLength);

    qint16 ret;
    qint16 consumedBytes(0);
    bool convOk;

    //parse object identifier
    ret = bParser.parseNext();
    objId = bParser.toObjectId(&convOk);
    if (ret < 0 || !bParser.isContextTag(0))
        return -1;
    consumedBytes += ret;

    //parse property identifier
    ret = bParser.parseNext();
    propertyId = (BacnetPropertyNS::Identifier)bParser.toUInt(&convOk);
    if (ret < 0 || !bParser.isContextTag(1))
        return -2;
    consumedBytes += ret;

    //parse OPTIONAL array index
    ret = bParser.parseNext();
    if (0 != ret) {//there is something - index or error
        arrayIndex = bParser.toUInt(&convOk);
        if (ret <0 || !bParser.isContextTag(2))
            return -3;
        consumedBytes += ret;
    } else {
        arrayIndex = Bacnet::ArrayIndexNotPresent;
    }

    Q_ASSERT(consumedBytes == bufferLength);
    if (consumedBytes != bufferLength) {
        return -BacnetRejectNS::ReasonTooManyArguments;
    }

    return consumedBytes;
}

qint32 ReadPropertyServiceData::toRaw(quint8 *startPtr, quint16 bufferLength)
{
    Q_CHECK_PTR(startPtr);
    quint8 *actualPtr(startPtr);
    quint16 leftLength(bufferLength);
    qint32 ret;

    //! \todo There is some duplication between Request and Ack toRaw() functions. Unify it.
    //encode Object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, objId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadProperty::toRaw()", "Cannot encode objId");
        qDebug("BacnetReadProperty::toRaw() : propertyencoding problem: objId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode proeprty identifier
    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, propertyId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadProperty::toRaw()", "Cannot encode propId");
        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode array index if present - OPTIONAL
    if (Bacnet::ArrayIndexNotPresent != arrayIndex) {//present
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, arrayIndex, true, 2);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode arrayIndex");
            qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    return actualPtr - startPtr;
}

//#define K_RP_TEST
#ifdef K_RP_TEST
int main()
{
    quint8 rpData[] = {
        0x0c,
        0x00, 0x00, 0x00, 0x05,
        0x19,
        0x55
    };
    const quint32 rpDataSize = sizeof rpData;

    ReadPropertyServiceData rpServData;
    qint32 ret = rpServData.fromRaw(rpData, rpDataSize);

    Q_ASSERT(ret == rpDataSize);
    Q_ASSERT(rpServData.objId.instanceNum == 5);
    Q_ASSERT(rpServData.objId.objectType == BacnetObjectType::AnalogInput);
    Q_ASSERT(rpServData.propertyId == BacnetProperty::PresentValue);

    quint32 writeSize(64);
    quint8 writeRpData[writeSize];

    ret = rpServData.toRaw(writeRpData, writeSize);

    Q_ASSERT(ret == rpDataSize);
    Q_ASSERT(memcmp(rpData, writeRpData, rpDataSize) == 0);

    HelperCoder::printArray(rpData, rpDataSize, "Original: \t");
    HelperCoder::printArray(writeRpData, ret, "Written:\t");

    qDebug("TEST SUCCEEDED");

    return 0;
}
#endif
