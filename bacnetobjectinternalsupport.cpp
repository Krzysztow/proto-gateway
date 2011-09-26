#include "bacnetobjectinternalsupport.h"

#include "bacnetdeviceobject.h"

BacnetObjectInternalSupport::~BacnetObjectInternalSupport()
{
    qDeleteAll(_cdmProperties);
}

bool BacnetObjectInternalSupport::addInternalProperty(BacnetProperty::Identifier propertyType, Property *property)
{
    if (_cdmProperties.contains(propertyType))
        return false;

    property->setOwner(this);
    _cdmProperties.insert(propertyType, property);
    return true;
}

BacnetProperty::Identifier BacnetObjectInternalSupport::findPropertyIdentifier(Property *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    QMap<BacnetProperty::Identifier, Property *>::Iterator propertyIt = _cdmProperties.begin();
    for (; propertyIt != _cdmProperties.end(); ++propertyIt) {
        if (propertyIt.value() == toBeGotten)
            return propertyIt.key();
    }
    Q_ASSERT(_cdmProperties.end() != propertyIt);
    return BacnetProperty::UndefinedProperty;
}

int BacnetObjectInternalSupport::getPropertyRequest(PropertySubject *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    Q_UNUSED(toBeGotten);
    Q_ASSERT_X(false, "BacnetObjectInternalSupport::getPropertyRequest()", "This function shouldn't be called here! The properties here should always be observer ones.");
    return Property::UnknownError;
}

int BacnetObjectInternalSupport::setPropertyRequest(PropertySubject *toBeSet, QVariant &value)
{
    Q_CHECK_PTR(toBeSet);
    Q_UNUSED(value);
    Q_UNUSED(toBeSet);
    Q_ASSERT_X(false, "BacnetObjectInternalSupport::setPropertyRequest()", "This function shouldn't be called here! The properties here should always be observer ones.");
    return Property::UnknownError;
}
