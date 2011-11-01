#include "bacnetcovsupport.h"

using namespace Bacnet;

BacnetCovSupport::BacnetCovSupport()
{
}

CovSubscription::CovSubscription(Bacnet::SubscribeCOVServiceData &data, BacnetAddress &address):
    _subscriberProcId(data._subscriberProcId),
    _monitoredObjectId(data._monitoredObjectId),
    _issueConfNotification(data._issueConfNotification),
    _subscriberAddress(address),
    _timeLeft(data._lifetime)
{
}

bool CovSubscription::compareSubscriptions(SubscribeCOVServiceData &subscription)
{
    return ( (_subscriberProcId == subscription._subscriberProcId) &&
             (_monitoredObjectId == subscription._monitoredObjectId) );
}


//////////////////////////////////////////////////

qint32 CovSubscription::toRaw(quint8 *ptrStart, quint16 buffLength)
{

}

qint32 CovSubscription::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{

}

qint32 CovSubscription::fromRaw(BacnetTagParser &parser)
{

}

qint32 CovSubscription::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{

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
