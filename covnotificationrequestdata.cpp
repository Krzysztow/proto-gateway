#include "confcovnotificationrequestdata.h"

CovNotificationRequestData::CovNotificationRequestData()
{
}

qint16 CovNotificationRequestData::fromRaw(quint8 *serviceData, quint16 buffLength)
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
    quint8 *actualPtr(serviceData);
    qint16 ret(0);

    //set process id
    //! \todo We know it's 1byte long. Maybe just write it, without BacnetCoder help?
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _subscribProcess, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot encode process id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode process id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //set object identifier
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _monitoredObjectId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "CovNotificationRequestData::toRaw()", "Cannot encode init obj id.");
        qDebug("CovNotificationRequestData::toRaw() : Cannot encode init obj id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    buffLength -= ret;

    //set object identifier
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _monitoredObjectId, true, 2);
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

    return actualPtr - serviceData;
}
