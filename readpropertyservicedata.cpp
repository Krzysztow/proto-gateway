#include "readpropertyservicedata.h"

#include "bacnetcoder.h"
#include "bacnettagparser.h"

using namespace Bacnet;

ReadPropertyServiceData::ReadPropertyServiceData(ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                          quint32 arrayIndex)
{
    this->objId = objId;
    this->propertyId = propertyId;
    this->arrayIndex = arrayIndex;
}

ReadPropertyServiceData::ReadPropertyServiceData()
{
}

qint32 ReadPropertyServiceData::fromRaw(quint8 *serviceData, quint8 buffLength)
{
    BacnetTagParser bParser(serviceData, buffLength);

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
    propertyId = (BacnetProperty::Identifier)bParser.toUInt(&convOk);
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

    Q_ASSERT(consumedBytes == buffLength);
    if (consumedBytes != buffLength) {
        return BacnetReject::ReasonTooManyArguments;
    }

    return consumedBytes;
}

qint32 ReadPropertyServiceData::toRaw(quint8 *startPtr, quint8 buffLength)
{
    Q_CHECK_PTR(startPtr);
    quint8 *actualPtr(startPtr);
    quint16 leftLength(buffLength);
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
