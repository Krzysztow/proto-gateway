#include "covnotificationrequestdata.h"

CovNotificationRequestData::CovNotificationRequestData()
{
}

qint32 CovNotificationRequestData::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    quint8 *actualPtr(serviceData);
    qint16 ret(0);
    bool convOk;

    BacnetTagParser bParser(serviceData, buffLength);

    //get process id
    ret = bParser.parseNext();
    _subscribProcess = bParser.toUInt(&convOk);
    if (ret <= 0 || !bParser.isContextTag(0) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get object identifier
    ret = bParser.parseNext();
    _initiatingObjectId = bParser.toObjectId(&convOk);
    if (ret <= 0 || !bParser.isContextTag(1) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get object identifier
    ret = bParser.parseNext();
    _monitoredObjectId = bParser.toObjectId(&convOk);
    if (ret <= 0 || !bParser.isContextTag(2) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get time remaining
    ret = bParser.parseNext();
    _timeLeft = bParser.toUInt(&convOk);
    if (ret <= 0 || !bParser.isContextTag(3) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    ret = _listOfValues.fromRawSpecific(bParser, 4, _monitoredObjectId.objectType);
    if (ret <= 0)
        return -2;

    actualPtr += ret;

    //no more needed to be parsed. The property value is object specific and will be extracted when executed,
    return actualPtr - serviceData;
}

qint32 CovNotificationRequestData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    quint8 *actualPtr(startPtr);
    qint16 ret(0);

    //set process id
    //! \todo We know it,s 1byte long. Maybe just write it, without BacnetCoder help?
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _subscribProcess, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot encode process id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode process id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //set object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, _initiatingObjectId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot encode init obj id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode init obj id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //set object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, _monitoredObjectId, true, 2);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot encode monitored obj id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode monitored obj id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;


    //set time remaining
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _timeLeft, true, 3);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot encode time left id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode time left id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //encode list of values
    ret = _listOfValues.toRaw(actualPtr, buffLength, 4);
    if (ret <= 0) {
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot values id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode values: %d", ret);
        return -ret;
    }

    actualPtr += ret;

    return (actualPtr - startPtr);
}

#include <QBitArray>
#include "bacnetprimitivedata.h"
#include "propertyvalue.h"
int main()
{
    //0x09 12 1C 02 00 00 04 2C 00 00 00 0A 39 00 4E 09 55 2E 44 42 82 00 00 2F 09 6F 2E 82 04 00 2F 4F
    quint8 covNotifData[] = {
        0x09,
        0x12,
        0x1C,
        0x02, 0x00, 0x00, 0x04,
        0x2C,
        0x00, 0x00, 0x00, 0x0A,
        0x39,
        0x00,
        0x4E,
        0x09,
        0x55,
        0x2E,
        0x44,
        0x42, 0x82, 0x00, 0x00,
        0x2F,
        0x09,
        0x6F,
        0x2E,
        0x82,
        0x04, 0x00,
        0x2F,
        0x4F
    };

    const int size = sizeof(covNotifData);

    CovNotificationRequestData covData;
    qint32 ret = covData.fromRaw(covNotifData, size);
    Q_ASSERT(ret == size);
    qDebug()<<"subscriber:"<<covData._subscribProcess<<"\n"<<
            "Monitored obj id:"<<objIdToNum(covData._monitoredObjectId)<<"\n"<<
            "Time:"<<covData._timeLeft<<"\n"<<
            "PV:"<<covData._listOfValues._sequence.at(0)->_value.value->toInternal()<<"\n"<<
            "SF:"<<covData._listOfValues._sequence.at(1)->_value.value->toInternal()<<"\n";

    CovNotificationRequestData covData2;
    covData2._subscribProcess = 18;
    covData2._initiatingObjectId = numToObjId( (BacnetObjectType::Device << 22) | 0x04);
    covData2._monitoredObjectId = numToObjId( (BacnetObjectType::AnalogInput << 22) | 0x0a);
    covData2._timeLeft = 0x00;

    Bacnet::Real *v = new Bacnet::Real(65.0);
    covData2._listOfValues.append(new PropertyValue(BacnetProperty::PresentValue, v));

    Bacnet::BitString *bitString = new Bacnet::BitString();
    QBitArray &bits = bitString->value();
    bits.resize(4);
    bits.setBit(0, false);
    bits.setBit(1, false);
    bits.setBit(2, false);
    bits.setBit(3, false);
    covData2._listOfValues.append(new PropertyValue(BacnetProperty::StatusFlags, bitString));

    const quint16 l(64);
    quint8 rawData[l];

    ret = covData2.toRaw(rawData, l);

    HelperCoder::printArray(rawData, ret, "Returned array:");

    Q_ASSERT(ret == size);
    Q_ASSERT(0 == memcmp(covNotifData, rawData, ret));
}

