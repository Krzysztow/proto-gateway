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

public://property owner methods.
    /** These two functions shouldn't be ever called when it comes to owners having only PropertyObservers! Here I override
    them with empty bodies.
      */
    virtual int getPropertyRequest(PropertySubject *toBeGotten);
    virtual int setPropertyRequest(PropertySubject *toBeSet, QVariant &value);
//    virtual void propertyValueChanged(PropertyObserver *property) = 0;
//    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)  =  0;

protected://helper functions
    BacnetProperty::Identifier findPropertyIdentifier(PropertySubject *toBeGotten);

protected:
    QMap<BacnetProperty::Identifier, Property*> _cdmProperties;
};

#endif // BACNETOBJECTINTERNALSUPPORT_H
