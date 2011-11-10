#ifndef BACNETDEVICEOBJECT_H
#define BACNETDEVICEOBJECT_H

#include <QtCore>

#include "bacnetobject.h"
#include "bacnetcommon.h"
#include "bacnetdata.h"
#include "bacnetobjectinternalsupport.h"

class InternalObjectsHandler;

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
    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(BacnetProperty::Identifier propId, quint32 arrayIdx, Bacnet::Error *error);
    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(Bacnet::ReadPropertyServiceData *rpStruct, Bacnet::Error *error);
    virtual int ensurePropertyReadySet(Bacnet::PropertyValueStruct &writeData, Bacnet::Error *error);

    virtual const QList<BacnetProperty::Identifier> &covProperties();

public://functions specific to BACnet device
    BacnetObject *bacnetObject(quint32 instanceNumber);
    bool addBacnetObject(BacnetObject *object);
    void propertyIoFinished(int asynchId, int result, BacnetObject *object);
    void setHandler(InternalObjectsHandler *bHandler);
    const QMap<quint32, BacnetObject*> &childObjects();
    Bacnet::BacnetDataInterface *constProperty(BacnetProperty::Identifier propertyId);
    void propertyValueChanged(BacnetObject *object, BacnetProperty::Identifier propId);

public://functions overridden from PropertyOwner
    /** Hook function that is called after having requested reading/writting a property (which obviously doesn't
        belong to Bacnet). Here we find, which property it is, add some BACnet specific parameters and propagate
        them upwards to device, which calls Protocol handler.
      */
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    virtual void propertyValueChanged(Property *property);

private:
    QVariant::Type variantTypeForProperty_helper(BacnetProperty::Identifier propertyId);
    Bacnet::BacnetDataInterface *createBacnetTypeForProperty_helper(BacnetProperty::Identifier propertyId, quint32 arrayIdx);

    /*public for a while
private:*/
public:
    Bacnet::ObjectIdStruct _id;
    typedef QMap<BacnetProperty::Identifier, Bacnet::BacnetDataInterface*> TPropertiesMap;
    TPropertiesMap _specializedProperties;
//    QMap<BacnetProperty::Identifier, Property *> _cdmProperties;

    QMap<quint32, BacnetObject*> _childObjects;
    InternalObjectsHandler *_handler;
};

#endif // BACNETDEVICEOBJECT_H
