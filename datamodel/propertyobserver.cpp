#include "propertyobserver.h"

#include "propertyowner.h"
#include "propertysubject.h"

///////////////PropertObserver

PropertyObserver::PropertyObserver(PropertyOwner *container, PropertySubject *property):
        _property(property),
        _owner(container)
{
    Q_ASSERT(_property);
    _property->addObserver(this);
}

PropertyObserver::~PropertyObserver()
{
    _property->rmObserver(this);
}

void PropertyObserver::setOwner(PropertyOwner *owner)
{
    _owner = owner;
}

PropertyOwner *PropertyObserver::owner()
{
    return _owner;
}

int PropertyObserver::getValue(QVariant *outValue)
{
    return _property->getValueSafe(outValue, this);
}

QVariant::Type PropertyObserver::type()
{
    return _property->type();
}

int PropertyObserver::setValue(QVariant &inValue)
{
    return _property->setValueSafe(inValue, this);
}

void PropertyObserver::propertyValueChanged()
{
    _owner->propertyValueChanged(this);
}

int PropertyObserver::getValueInstant(QVariant *outValue)
{
    Q_ASSERT(_property);
    //we know it's not Property, but PropertySubject, so no virtual function is called, but straight PropertySubject one.
    return _property->getValue(outValue);
}

void PropertyObserver::asynchActionFinished(int asynchId, Property::ActiontResult actionResult)
{
    if (0 != _owner)
        _owner->asynchActionFinished(asynchId, this, actionResult);
}
