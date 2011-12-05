#include "sngsimpleactorproperty.h"

#include "sngcommon.h"
#include "propertysubject.h"

using namespace Sng;

SngSimpleActorProperty::SngSimpleActorProperty(PropertySubject *subject,
                                               ConnectionFrame::DataType txAddrType, GroupAddress txAddress,
                                               ConnectionFrame::DataType rxAddrType, GroupAddress &rxAddress):
    SngInternalSupport(rxAddress, rxAddrType, txAddress, txAddrType),
    _property(subject)
{
    Q_CHECK_PTR(_property);
    _property->setOwner(this);

    /* If feedbackAddr is null, that means at this point we listen to no frame. This is kind of problematic, since we would have no feedback
      from the actor - no way to know, its property value changed. So we have to cheat a bit and assume, that all the requests send with SET
      address correspond to the value change.
      Of course it's a simplification and it could happen that device doesnt response to requests. But there is no other way to get current status.
      SNG doesn't provide read ability.
      */

    if (!_rxAddress.isValid()) {
        _rxAddress = _txAddress;
        _rxType = _txType;
        registerForAddress(_rxAddress);
    }
}

void SngSimpleActorProperty::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(actionResult);
    Q_CHECK_PTR(_property);
    Q_ASSERT(false);//we own PropertySubject, this can't be invoked.
}

void SngSimpleActorProperty::propertyValueChanged(Property *property)
{
    Q_UNUSED(property);
    Q_ASSERT(false);//we have property subject, this can't be invoked
}

int SngSimpleActorProperty::getPropertyRequested(PropertySubject *toBeGotten)
{
    Q_UNUSED(toBeGotten);
    Q_ASSERT(_property == toBeGotten);

    if (toBeGotten->isNull())
        return Property::NotSetYet;
    else
        return Property::ResultOk;
}

int SngSimpleActorProperty::setPropertyRequested(PropertySubject *toBeSet, QVariant &value)
{
    Q_ASSERT(toBeSet == _property);
    if (toBeSet != _property)
        return Property::UnknownProperty;

    if (!_txAddress.isValid()) {
        qDebug("%s : Adderess not valid", __PRETTY_FUNCTION__);
        return Property::UnknownError;
    }

    if (sendFrameWithVariant_helper(_txType, _txAddress, value))
        return Property::ResultOk;
    else
        return Property::UnknownError;
}

void SngSimpleActorProperty::frameWithVariantReceived_hook(QVariant &propertyValue)
{
    _property->setValue(propertyValue);
}
