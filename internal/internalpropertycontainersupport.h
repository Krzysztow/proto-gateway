#ifndef INTERNALPROPERTYCONTAINERSUPPORT_H
#define INTERNALPROPERTYCONTAINERSUPPORT_H

#include <QtCore>
#include "bacnetcommon.h"
#include "property.h"

namespace Bacnet {

class BacnetProperty;
class BacnetObject;
class BacnetDeviceObject;

class InternalPropertyContainerSupport
{
public:
    //! Hook method which is called by property (or propagated further), when action on internal proeprty is done.
//    virtual void propertyAsynchActionFinished(BacnetPropertyNS::Identifier propId, quint32 arrayIndex, int asynchId, ::Property::ActiontResult result) = 0;
    virtual void propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                              BacnetProperty *subProperty = 0, BacnetProperty *mainProperty = 0,
                                              BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0) = 0;
};

}

#endif // INTERNALPROPERTYCONTAINERSUPPORT_H
