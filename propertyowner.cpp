#include "propertyowner.h"
#include "property.h"

PropertyOwner::~PropertyOwner()
{
}

int PropertyOwner::getPropertyRequest(Property *toBeGotten, Property *requester)
{
    Q_UNUSED(requester);
    Q_UNUSED(toBeGotten);
    return Property::Ready;
}

int PropertyOwner::setPropertyRequest(Property *toBeSet, QVariant &value, Property *requester)
{
    Q_UNUSED(requester);
    Q_UNUSED(toBeSet);
    Q_UNUSED(value);
    return Property::Ready;
}

void PropertyOwner::setRequestResult(int asynchId, Property *property, QVariant &propValue, bool success)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(propValue);
    Q_UNUSED(success);
}

void PropertyOwner::getRequestResult(int asynchId, Property *property, QVariant &propValue, bool success)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(propValue);
    Q_UNUSED(success);
}
