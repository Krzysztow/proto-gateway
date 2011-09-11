#ifndef BACNETDEVICEOBJECT_H
#define BACNETDEVICEOBJECT_H

#include <QtCore>

#include "bacnetobject.h"
#include "bacnetcommon.h"
#include "bacnetdata.h"
#include "bacnetobjectinternalsupport.h"

class AsynchSetter;
class BacnetDeviceObject:
        public BacnetObject,
        public BacnetObjectInternalSupport
{
public:
    BacnetDeviceObject(Bacnet::ObjectIdStruct identifier);
    BacnetDeviceObject(quint32 instanceNumber);
    ~BacnetDeviceObject();

public://overridden from BacnetObject
    virtual int ensurePropertyReadyRead(BacnetProperty::Identifier propertyId);

    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(Bacnet::ReadPropertyStruct *rpStruct, Bacnet::Error *error);

    virtual int ensurePropertyReadySet(Bacnet::PropertyValueStruct &writeData, Bacnet::Error *error);

public://functions specific to BACnet device
    BacnetObject *bacnetObject(quint32 instanceNumber);
    bool addBacnetObject(BacnetObject *object);
    void propertyChanged(int asynchId, int result, BacnetObject *object);
    void setHandler(AsynchSetter *bHandler);

public://functions overridden from PropertyOwner
    /** This function is a hook. Is invoked whenever some foreign protocol asks the property for a value. We have
      to decide if it's going to be read at once or some asynchronous action called (of our Bacnet property). The
      decision is based directed to the device and from there to protocol, which really makes the decision.
        \sa PropertyOwner::getPropertyRequest()
      */
    virtual int getPropertyRequest(PropertySubject *toBeGotten);

    /** Similiar to \sa getPropertyRequest() but a foreign protocol tries to write to the linked BACnet property
        instead.
      */
    virtual int setPropertyRequest(PropertySubject *toBeSet, QVariant &value);

    /** Hook function that is called after having requested reading/writting a property (which obviously doesn't
        belong to Bacnet). Here we find, which property it is, add some BACnet specific parameters and propagate
        them upwards to device, which calls Protocol handler.
      */
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);

private:
    QVariant::Type variantTypeForProperty_helper(BacnetProperty::Identifier propertyId);
    Bacnet::BacnetDataInterface *createBacnetTypeForProperty_helper(BacnetProperty::Identifier propertyId, quint32 arrayIdx);

    /*public for a while
private:*/
public:
    Bacnet::ObjectIdStruct _id;
    QMap<BacnetProperty::Identifier, Bacnet::BacnetDataInterface*> _specializedProperties;
//    QMap<BacnetProperty::Identifier, Property*> _cdmProperties;

    QMap<quint32, BacnetObject*> _childObjects;
    AsynchSetter *_handler;
};

#endif // BACNETDEVICEOBJECT_H
