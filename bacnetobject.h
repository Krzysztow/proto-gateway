#ifndef BACNETOBJECT_H
#define BACNETOBJECT_H

#include <QtCore>

#include "bacnetcommon.h"
#include "bacnetprimitivedata.h"
#include "covsupport.h"
#include "internalpropertycontainersupport.h"

struct WriteAccessSpecificationStruct;
class BacnetDataBase;
class BacnetAddress;

namespace Bacnet{
    class BacnetDeviceObject;
    class Error;
    class ReadPropertyServiceData;
    class PropertyValue;
    class SubscribeCOVServiceData;
    class BacnetProperty;
}

class Property;

namespace Bacnet {

class BacnetObject:
        public CovSupport,
        public InternalPropertyContainerSupport
{
public:
    BacnetObject(ObjectIdentifier &id, BacnetDeviceObject *parentDevice);
    BacnetObject(BacnetObjectTypeNS::ObjectType objectType, quint32 instanceNumber, BacnetDeviceObject *parentDevice);
    virtual ~BacnetObject();


    //! Used to check if we can read from the device. If not yet, the asynchronous id for read request should be returned or error status.
    virtual int propertyReadTry(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Error *error = 0);

    //! Returns the data associated with the propertyId.
    virtual BacnetDataInterfaceShared propertyReadInstantly(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Error *error = 0);

    virtual int propertySet(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Error *error = 0);

    bool readClassDataHelper(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Error *error = 0);

    const ObjectIdentifier &objectId() const;
    quint32 objectIdNum() const;

    void setObjectName(QString name);
    QString objectName() const;

    /** Adds property to the property list. If property is already used, nothing happens, but returns false.
        To replace property call \sa removeProperty/\sa takeProperty and then addProperty.
        \note Note that array properties are supposed to be entered and taken as a whole.
      */
    bool addProperty(BacnetPropertyNS::Identifier identifier, BacnetProperty *property);
    //! Deletes property from list (destructor is invoked). If property found, returns true. Otherwise false.
    bool removeProperty(BacnetPropertyNS::Identifier identifier);
    //! Removes property from properties list and returns. If not found, 0 poitner is returned.
    BacnetProperty *takeProperty(BacnetPropertyNS::Identifier identifier);

    const QMap<BacnetPropertyNS::Identifier, BacnetProperty*> &objProperties() const;
    BacnetPropertyNS::Identifier propertyIdendifier(BacnetProperty *property) const;

public://method overriden from InternalPropertyContainerSupport
    virtual void propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                              BacnetProperty *property, ArrayProperty *arrayProperty,
                                              BacnetObject *parentObject, BacnetDeviceObject *deviceObject);


    virtual void propertyValueChanged(BacnetProperty *property = 0, ArrayProperty *arrayProperty = 0,
                                      BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0);

public://methods for CovSupport
    virtual const QList<BacnetPropertyNS::Identifier> covProperties();

private:
    ObjectIdentifier _id;
    QString _name;

protected:
    BacnetDeviceObject *_parentDevice;
    QMap<BacnetPropertyNS::Identifier, BacnetProperty*> _properties;
};

}

#endif // BACNETOBJECT_H
