#include "bacnetcovsubscription.h"

#include "subscribecovservicedata.h"
#include "bacnettagparser.h"

using namespace Bacnet;

CovSubscription::CovSubscription(SubscribeCOVServiceData &data, BacnetAddress &address):
    _recipientProcess(address, data._subscriberProcId),
    _monitoredPropertyRef(data._monitoredObjectId),
    _issueConfNotification(data._issueConfNotification),
    _timeLeft(data._lifetime),
    _covIncrement(data.takeCovIncrement())
{
}

CovSubscription::~CovSubscription()
{
    delete _covIncrement;
    _covIncrement = 0;
}

bool CovSubscription::compareSubscriptions(CovSubscription &subscription)
{
    return ( this->_recipientProcess == subscription._recipientProcess &&
             this->_monitoredPropertyRef == subscription._monitoredPropertyRef);
}

bool CovSubscription::compareParametrs(BacnetAddress &recipientAddress, quint32 recipientProcessId,
                                       ObjectIdentifier &objectId, BacnetProperty::Identifier propertyId, quint32 propertyArrayIdx)
{
    return ( (_recipientProcess.compare(recipientAddress, recipientProcessId) &&
              (_monitoredPropertyRef.compareParameters(objectId, propertyId, propertyArrayIdx))) );
}

void CovSubscription::update(quint32 lifetime, CovRealIcnrementHandler *covIncrement)
{
    _timeLeft = lifetime;
    //remove old increment
    if (0 != _covIncrement)
        delete _covIncrement;

    _covIncrement = covIncrement;
}

qint32 CovSubscription::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    quint8 *actualPtr(ptrStart);
    qint32 total(0), ret;

    //code recipient
    ret = _recipientProcess.toRaw(actualPtr, buffLength, 0);
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
    if (0 != _covIncrement) {
        ret = _covIncrement->toRaw(actualPtr, buffLength, 4);
        if (ret < 0) {
            qDebug("%s : error while coding time remaining", __PRETTY_FUNCTION__);
            return -5;
        }
        total += ret;
        actualPtr += ret;
        buffLength -= ret;
    }

    return total;
 }

qint32 CovSubscription::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return toRawTagEnclosed_helper(ptrStart, buffLength, tagNumber);
}

qint32 CovSubscription::fromRaw(BacnetTagParser &parser)
{
    qint32 ret(0), total(0);
    bool okOrCtxt;

    //decode recipient
    ret = _recipientProcess.fromRaw(parser, 0);
    if (ret < 0)
        return ret;
    total += ret;

    //decode monitored property refference
    ret = _monitoredPropertyRef.fromRaw(parser, 1);
    if (ret < 0)
        return ret;
    total += ret;

    //decode information aobut confirmed notifications
    ret = parser.parseNext();
    _issueConfNotification = parser.toBoolean(&okOrCtxt);
    if ((ret < 0) || !okOrCtxt || !parser.isContextTag(2) )
        return BacnetError::CodeMissingRequiredParameter;
    total += ret;

    //decode time remaining
    ret = parser.parseNext();
    _timeLeft = parser.toUInt(&okOrCtxt);
    if ( (ret < 0) || !okOrCtxt || !parser.isContextTag(3) )
        return BacnetError::CodeMissingRequiredParameter;
    total += ret;

    //cov increment?
    ret = parser.nextTagNumber(&okOrCtxt);
    if ( okOrCtxt && (4 == ret) ) {//has increment
        if (0 == _covIncrement)
            _covIncrement = new CovRealIcnrementHandler();

        ret = _covIncrement->fromRaw(parser, 4);
        if (ret < 0)
            return ret;
        total += ret;
    }

    return total;
}

qint32 CovSubscription::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return fromRawTagEnclosed_helper(parser, tagNum);
}

bool CovSubscription::setInternal(QVariant &value)
{
    Q_UNUSED(value);
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

bool CovSubscription::isCovPropertySubscription(BacnetProperty::Identifier propId, quint32 propertyArrayIdx)
{
    return _monitoredPropertyRef.compareParameters(_monitoredPropertyRef.objId(), propId, propertyArrayIdx);
}
