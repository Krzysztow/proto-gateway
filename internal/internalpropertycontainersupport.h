#ifndef INTERNALPROPERTYCONTAINERSUPPORT_H
#define INTERNALPROPERTYCONTAINERSUPPORT_H

#include <QtCore>
#include "bacnetcommon.h"
#include "property.h"

namespace Bacnet {

class BacnetProperty;
class ArrayProperty;
class BacnetObject;
class BacnetDeviceObject;

class InternalPropertyContainerSupport
{
public:
    //! Hook method which is called by property (or propagated further), when action on internal proeprty is done.
    virtual void propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                              BacnetProperty *property = 0, ArrayProperty *arrayProperty = 0,
                                              BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0) = 0;

    //! Hook method which is called by property (or propagated further), when value of the internal property has been changed.
    virtual void propertyValueChanged(BacnetProperty *property = 0, ArrayProperty *arrayProperty = 0,
                                      BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0) = 0;
};

}

#endif // INTERNALPROPERTYCONTAINERSUPPORT_H

/*propertyValueChanged(BacnetProperty *property = 0, BacnetArray *arrayProperty = 0,
                                      BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0)*/
