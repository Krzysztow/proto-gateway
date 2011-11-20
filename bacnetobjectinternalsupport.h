#ifndef BACNETOBJECTINTERNALSUPPORT_H
#define BACNETOBJECTINTERNALSUPPORT_H

#include <QtCore>

#include "propertyowner.h"
#include "bacnetcommon.h"


//class BacnetObject;
//namespace Bacnet {class BacnetDeviceObject;}

class BacnetObjectInternalSupport:
        public PropertyOwner
{
public:
    virtual ~BacnetObjectInternalSupport();

public:
    bool addInternalProperty(BacnetPropertyNS::Identifier propertyType, Property *property);

public://property owner methods.
    /** These two functions shouldn't be ever called when it comes to owners having only PropertyObservers! Here I override
    them with empty bodies.
      */
    virtual int getPropertyRequested(PropertySubject *toBeGotten);
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);
//    virtual void propertyValueChanged(Property *property) = 0;
//    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)  =  0;

protected://helper functions
    BacnetPropertyNS::Identifier findPropertyIdentifier(Property *toBeGotten);

protected:
    QMap<BacnetPropertyNS::Identifier, Property *> _cdmProperties;
};

#endif // BACNETOBJECTINTERNALSUPPORT_H
