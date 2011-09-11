#ifndef BACNETOBJECTINTERNALSUPPORT_H
#define BACNETOBJECTINTERNALSUPPORT_H

#include <QtCore>

#include "propertyowner.h"
#include "bacnetcommon.h"

class BacnetObject;
class BacnetDeviceObject;
class BacnetObjectInternalSupport:
        public PropertyOwner
{
public:
    virtual ~BacnetObjectInternalSupport();

public:
    bool addInternalProperty(BacnetProperty::Identifier propertyType, Property *property);

protected://helper functions
    BacnetProperty::Identifier findPropertyIdentifier(PropertySubject *toBeGotten);

protected:
    QMap<BacnetProperty::Identifier, Property*> _cdmProperties;
};

#endif // BACNETOBJECTINTERNALSUPPORT_H
