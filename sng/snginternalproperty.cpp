#include "snginternalproperty.h"

#include "propertyobserver.h"

using namespace Sng;

#define RETURN_IF_WRONG_TYPE(expected, gotten)                      \
    if (expected !=  gotten) {                                      \
        Inform_Wrong_Type(expected, gotten);                        \
        return;                                                     \
    }

#ifdef DEBUG_TYPE
#define Inform_Wrong_Type(a, b) informWrongType(a, b);
void SngInternalProperty::informWrongType(ConnectionFrame::DataType typeExpected, ConnectionFrame::DataType typeGotten)
{
    qDebug("%s : received %d, whereas am looking only for %d type", __PRETTY_FUNCTION__, typeGotten, typeExpected);
}
#else
#define Inform_Wrong_Type(a, b) ;
#endif

SngInternalProperty::SngInternalProperty():
    _property(0)
{
}

int SngInternalProperty::getPropertyRequested(PropertySubject *toBeGotten)
{
    Q_UNUSED(toBeGotten);
    Q_ASSERT(false);

    return Property::UnknownError;
}

int SngInternalProperty::setPropertyRequested(PropertySubject *toBeSet, QVariant &value)
{
    Q_UNUSED(toBeSet);
    Q_UNUSED(value);
    Q_ASSERT(false);

    return Property::UnknownError;
}

void SngInternalProperty::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(actionResult);
    qDebug("%s : got resulr %d, don't care!", __PRETTY_FUNCTION__, actionResult);
}

void SngInternalProperty::propertyValueChanged(Property *property)
{
    Q_UNUSED(property);
    qDebug("%s : got info, don't care!", __PRETTY_FUNCTION__);
}

void SngInternalProperty::setInternalProperty_hook(QVariant &propertyValue)
{
    Q_CHECK_PTR(_property);
    //in this simple implementation we try to set property and don't care what happened with it
    _property->setValue(propertyValue);
}

void SngInternalProperty::receiveOnOff(const GroupAddress &address, bool value)
{
    RETURN_IF_WRONG_TYPE(_setInternaCommandlType, ConnectionFrame::OnOff);

    Q_ASSERT(address == _setInternalCommandAddress);
    QVariant varValue(value);
    setInternalProperty_hook(varValue);
}

void SngInternalProperty::receiveDimm(const GroupAddress &address, ObjectConnectionSupport::DimmCommand value)
{
    RETURN_IF_WRONG_TYPE(_setInternaCommandlType, ConnectionFrame::Dimm);

     Q_ASSERT(address == _setInternalCommandAddress);
     QVariant varValue((quint8)value);
     setInternalProperty_hook(varValue);
}

void SngInternalProperty::receiveTirme(const GroupAddress &address, const QTime &value)
{
    RETURN_IF_WRONG_TYPE(_setInternaCommandlType, ConnectionFrame::Time);

    Q_ASSERT(address == _setInternalCommandAddress);
    QVariant varValue(value);
    setInternalProperty_hook(varValue);
}

void SngInternalProperty::receiveDate(const GroupAddress &address, const QDate &value)
{
    RETURN_IF_WRONG_TYPE(_setInternaCommandlType, ConnectionFrame::Date);

    Q_ASSERT(address == _setInternalCommandAddress);
    QVariant varValue(value);
    setInternalProperty_hook(varValue);
}

void SngInternalProperty::receiveTemp(const GroupAddress &address, float value)
{
    RETURN_IF_WRONG_TYPE(_setInternaCommandlType, ConnectionFrame::Temp);

    Q_ASSERT(address == _setInternalCommandAddress);
    QVariant varValue(QMetaType::Float);
    varValue.setValue(value);
    setInternalProperty_hook(varValue);
}

void SngInternalProperty::receiveValue(const GroupAddress &address, int value)
{
    RETURN_IF_WRONG_TYPE(_setInternaCommandlType, ConnectionFrame::Value);

     Q_ASSERT(address == _setInternalCommandAddress);
     QVariant varValue((quint8)value);
     setInternalProperty_hook(varValue);
}

