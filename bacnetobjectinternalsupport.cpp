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

BacnetProperty::Identifier BacnetObjectInternalSupport::findPropertyIdentifier(PropertySubject *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    QMap<BacnetProperty::Identifier, Property*>::Iterator propertyIt = _cdmProperties.begin();
    for (; propertyIt != _cdmProperties.end(); ++propertyIt) {
        if (propertyIt.value() == toBeGotten)
            return propertyIt.key();
    }
    Q_ASSERT(_cdmProperties.end() != propertyIt);
    return BacnetProperty::UndefinedProperty;
}
