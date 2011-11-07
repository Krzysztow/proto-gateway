#include "bacnetcovsupport.h"

#include "subscribecovservicedata.h"

using namespace Bacnet;

CovSubscription::CovSubscription(SubscribeCOVServiceData &data, BacnetAddress &address):
    _recipient(data._subscriberProcId, address),
    _monitoredPropertyRef(data._monitoredObjectId),
    _issueConfNotification(data._issueConfNotification),
    _timeLeft(data._lifetime),
    _covIncrement(data.takeCovIncrement())
{
}

bool CovSubscription::compareSubscriptions(CovSubscription &subscription)
{
    return ( this->_recipient == subscription._recipient &&
             this->_monitoredPropertyRef == subscription._monitoredPropertyRef);
}



qint32 CovSubscription::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint8 *actualPtr(ptrStart);
    qint32 total(0), ret;

    //code recipient
    ret = _recipient.toRaw(actualPtr, buffLength, 0);
    if (ret < 0) {
        qDebug("%s : error while coding recipient", __PRETTY_FUNCTION__);
        return -1;
    }
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //code property reference
    ret = _monitoredPropertyRef.toRaw(actualPtr, buffLength, 1);
    if (ret < 0) {
        qDebug("%s : error while coding monitored property reference", __PRETTY_FUNCTION__);
        return -2;
    }
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //code confirmed notifications
    ret = BacnetCoder::boolToRaw(actualPtr, buffLength, _issueConfNotification, true, 2);
    if (ret < 0) {
        qDebug("%s : error while coding issue confirmed notifications", __PRETTY_FUNCTION__);
        return -3;
    }
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //code time remaining
    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _timeLeft, true, 3);
    if (ret < 0) {
        qDebug("%s : error while coding time remaining", __PRETTY_FUNCTION__);
        return -4;
    }
    total += ret;
    actualPtr += ret;
    buffLength -= ret;

    //OPTIONAL
    //code COVIncrement
    if (_)
 }

qint32 CovSubscription::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 CovSubscription::fromRaw(BacnetTagParser &parser)
{

}

qint32 CovSubscription::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool CovSubscription::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, "CovSubscription::setInternal()", "Structured data types shouldn't be used for internal use!");
    return false;
}

QVariant CovSubscription::toInternal()
{
    Q_ASSERT_X(false, "CovSubscription::setInternal()", "Structured data types shouldn't be used for internal use!");
    return QVariant();
}

DataType::DataType CovSubscription::typeId()
{
    return DataType::BACnetCOVSubscription;
}

//////////////////////////////////////////////////

BacnetCovSupport::BacnetCovSupport()
{
}
