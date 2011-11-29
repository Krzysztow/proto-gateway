#include "wprequester.h"

WpRequester::WpRequester(Property *propertyToWrite, QVariant valueToWrite):
    _property(propertyToWrite),
    _value(valueToWrite)
{
    Q_CHECK_PTR(_property);
    _property->setOwner(this);
}

int WpRequester::getPropertyRequested(PropertySubject *toBeGotten)
{
    Q_UNUSED(toBeGotten);
    return Property::UnknownError;
}

int WpRequester::setPropertyRequested(PropertySubject *toBeSet, QVariant &value)
{
    Q_UNUSED(toBeSet);
    Q_UNUSED(value);
    Q_ASSERT(false);
    return Property::UnknownError;
}

void WpRequester::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(property);
    qDebug("%s : action with asynchronous id %d finished and with result %d", __PRETTY_FUNCTION__, asynchId, actionResult);
}

void WpRequester::propertyValueChanged(Property *property)
{
    Q_UNUSED(property);
    qDebug("%s : property value changed!", __PRETTY_FUNCTION__);
}

void WpRequester::writeValue()
{
    int result = _property->setValue(_value);
    qDebug("%s : Gotten asynch id/result: %d", __PRETTY_FUNCTION__, result);
}
