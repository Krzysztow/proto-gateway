#include "sngsimplesensorproperty.h"

#include "propertyobserver.h"
#include "snginternaltypesmapper.h"

using namespace Sng;

SngSimpleSensorProperty::SngSimpleSensorProperty(PropertyObserver *observer,
                                                 ConnectionFrame::DataType rxAddrType, GroupAddress &rxAddress,
                                                 ConnectionFrame::DataType txAddrType, GroupAddress &txAddress):
    SngInternalSupport(rxAddress, rxAddrType, txAddress, txAddrType),
    _property(observer)
{
    Q_CHECK_PTR(_property);
    _property->setOwner(this);
}

int SngSimpleSensorProperty::getPropertyRequested(PropertySubject *toBeGotten)
{
    Q_UNUSED(toBeGotten);
    qDebug("%s : shouldn't happen. PropertyObserver is owned.", __PRETTY_FUNCTION__);
    Q_ASSERT(false);//this should never be called, we own PropertyObserver

    return Property::UnknownError;
}

int SngSimpleSensorProperty::setPropertyRequested(PropertySubject *toBeSet, QVariant &value)
{
    Q_UNUSED(toBeSet);
    Q_UNUSED(value);
    qDebug("%s : shouldn't happen. PropertyObserver is owned.", __PRETTY_FUNCTION__);
    Q_ASSERT(false);//this should never be called, we own PropertyObserver

    return Property::UnknownError;
}

void SngSimpleSensorProperty::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(actionResult);
    qDebug("%s : got resulr %d, don't care!", __PRETTY_FUNCTION__, actionResult);
}

void SngSimpleSensorProperty::propertyValueChanged(Property *property)
{
    if (!_txAddress.isValid()) //we don't want to send any feedback. No feedback address set.
        return;

    Q_CHECK_PTR(property);
    QVariant value(SngInternalTypesMapper::sngTypeToInternal(_txType));
    if (!value.isValid()) {
        qDebug("%s : Can't find internal type for type %d", __PRETTY_FUNCTION__, _txType);
        return;
    }

    int ret = property->getValueInstant(&value);
    if (Property::ResultOk == ret)
        sendFrameWithVariant_helper(_txType, _txAddress, value);
    else {
        Q_ASSERT(ret <= Property::ResultOk);
        qDebug("%s : tried to read instanty and got %d", __PRETTY_FUNCTION__, ret);
    }
}

void SngSimpleSensorProperty::frameWithVariantReceived_hook(QVariant &propertyValue)
{
    Q_CHECK_PTR(_property);
    //in this simple implementation we try to set property and don't care what happened with it
    _property->setValue(propertyValue);
}

