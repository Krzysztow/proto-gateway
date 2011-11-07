#include "subscribecovservicedata.h"

#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

SubscribeCOVServiceData::SubscribeCOVServiceData():
    _subscriberProcId(0),
    _issueConfNotification(false),
    _lifetime(0),
    _propReference(0),
    _covIncrement(0),
    _flags(0)//nothing set yet
{
    _monitoredObjectId.objectType = BacnetObjectType::Undefined;
    _monitoredObjectId.instanceNum = Bacnet::InvalidInstanceNumber;
}

SubscribeCOVServiceData::~SubscribeCOVServiceData()
{
    clearHasPropertyReference();
    clearCovIncrement();
}


qint32 SubscribeCOVServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    Q_CHECK_PTR(startPtr);
    quint8 *actualPtr(startPtr);
    quint16 leftLength(buffLength);
    qint32 ret;

    //encode low range limit
    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _subscriberProcId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "SubscribeCOVServiceData::toRaw()", "Cannot encode process id.");
        qDebug("SubscribeCOVServiceData::toRaw() : Cannot encode process id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    //encode high range limit
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, _monitoredObjectId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "SubscribeCOVServiceData::toRaw()", "Cannot encode obj id.");
        qDebug("SubscribeCOVServiceData::toRaw() : Cannot encode obj id: %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    //encode issue confirmed notification, if present
    if (isConfirmedNotificationPresent()) {
        ret = BacnetCoder::boolToRaw(actualPtr, leftLength, _issueConfNotification, true, 2);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "SubscribeCOVServiceData::toRaw()", "Cannot encode bool.");
            qDebug("SubscribeCOVServiceData::toRaw() : Cannot encode bool: %d", ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    //encode lifetime
    if (isLifetimePresent()) {
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _lifetime, true, 3);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "SubscribeCOVServiceData::toRaw()", "Cannot encode lifetime.");
            qDebug("SubscribeCOVServiceData::toRaw() : Cannot encode lifetime: %d", ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    //if this is a sunscribeCovProperty request, it has more parameters to encode
    if (hasPropertyReference()) {
        ret = _propReference->toRaw(actualPtr, leftLength, 4);
        if (ret <= 0) {
            Q_ASSERT_X(false, "SubscribeCOVServiceData::toRaw()", "Cannot encode property ref.");
            qDebug("%s : Cannot encode prop ref: %d", __PRETTY_FUNCTION__, ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;

        if (hasCovIncrement()) {
            ret = _propReference->toRaw(actualPtr, leftLength, 5);
            if (ret <= 0) {
                Q_ASSERT_X(false, "SubscribeCOVServiceData::toRaw()", "Cannot encode cov increment.");
                qDebug("%s : Cannot encode cov increment: %d", __PRETTY_FUNCTION__, ret);
                return ret;
            }
            actualPtr += ret;
            leftLength -= ret;
        }
    }

    return (actualPtr - startPtr);
}

qint32 SubscribeCOVServiceData::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    Q_CHECK_PTR(serviceData);
    BacnetTagParser bParser(serviceData, buffLength);

    qint16 ret;
    qint16 consumedBytes(0);
    bool convOkOrCtxt;

    //parse process identifier
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isContextTag(0))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _subscriberProcId = bParser.toUInt(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;
    //monitored object id
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isContextTag(1))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _monitoredObjectId = bParser.toObjectId(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;
    //confirmed notifications flag OPTIONAL
    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if (2 == ret && convOkOrCtxt) {//has the flag
        ret = bParser.parseNext();
        _issueConfNotification = bParser.toBoolean(&convOkOrCtxt);
        if (!convOkOrCtxt)
            return -BacnetReject::ReasonInvalidParameterDataType;
        consumedBytes += ret;
        ret = bParser.nextTagNumber(&convOkOrCtxt);
        setConfirmedNotificationPresent();
    } else {
        clearConfirmedNotificationPresent();
    }
    //lifetime - optional
    if (3 == ret && convOkOrCtxt) {//we have lifetime present
        ret = bParser.parseNext();
        _lifetime = bParser.toUInt(&convOkOrCtxt);
        if (!convOkOrCtxt)
            return -BacnetReject::ReasonInvalidParameterDataType;
        consumedBytes += ret;
        setLifetimePresent();
    } else {
        clearLifetimePresent();
    }

    //in case of BACnet subscribeCOVProperty-Request we may have two additional fields
    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if (4 == ret && convOkOrCtxt) {//monitored property id (BacnetpropertyReference is there);
        if (0 == _propReference) _propReference = new Bacnet::PropertyReference();
        ret = _propReference->fromRaw(bParser, 4);
        if (ret < 0)
            return -BacnetReject::ReasonInconsistentParameters;
        consumedBytes += ret;

        ret = bParser.nextTagNumber(&convOkOrCtxt);
        //check for COV increment
        if ( (5 == ret) && convOkOrCtxt){
            Q_ASSERT(_propReference != 0);
            if (0 == _propReference)
                return -BacnetReject::ReasonInconsistentParameters;//whene COVIncrement is  provided, then this could happen only for subscribeCOV-Request

            if (0 == _covIncrement)  {
                _covIncrement = new CovRealIcnrementHandler();
            }

            ret = _covIncrement->fromRaw(bParser, 5);
            if (ret <= 0) //something worng, check it
                return -BacnetReject::ReasonInconsistentParameters;
            consumedBytes += ret;
        }
    } else {//if this was not
        clearHasPropertyReference();
        clearCovIncrement();
    }

    if (bParser.hasNext())
        return -BacnetReject::ReasonTooManyArguments;

    return consumedBytes;
}
