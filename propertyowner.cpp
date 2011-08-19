#include "propertyowner.h"
#include "property.h"

PropertyOwner::~PropertyOwner()
{
}

int PropertyOwner::getPropertyRequest(PropertySubject *toBeGotten)
{
    Q_UNUSED(toBeGotten);
    return Property::ResultOk;
}

int PropertyOwner::setPropertyRequest(PropertySubject *toBeSet, QVariant &value)
{
    Q_UNUSED(toBeSet);
    Q_UNUSED(value);
    return Property::ResultOk;
}

//void PropertyOwner::setRequestResult(int asynchId, bool success)
//{
//    Q_UNUSED(asynchId);
//    Q_UNUSED(success);
//}

//void PropertyOwner::getRequestResult(int asynchId, bool success)
//{
//    Q_UNUSED(asynchId);
//    Q_UNUSED(success);
//}

void PropertyOwner::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);//would be good to make a check that we really own the property
    Q_UNUSED(actionResult);
}
