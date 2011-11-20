#ifndef ANALOGINPUTOBJECT_H
#define ANALOGINPUTOBJECT_H

#include <QMap>

#include "bacnetdata.h"
#include "bacnetobject.h"
#include "bacnetobjectinternalsupport.h"
#include "covsupport.h"

class Property;
namespace Bacnet {

//class BacnetDeviceObject;
//class PropertyValue;

//class AnalogInputObject:
//        public BacnetObject,
//        public BacnetObjectInternalSupport
//{
//public:
//    AnalogInputObject(Bacnet::ObjectIdentifier &identifier, Bacnet::BacnetDeviceObject *parent);
//    AnalogInputObject(quint32 instanceNumber, Bacnet::BacnetDeviceObject *parent);
//    ~AnalogInputObject();

//public://functions overridden from BacnetObject
//    //! Used to check if we can read from the device. If not yet, the asynchronous id for read request should be returned or error status.
//    virtual int isPropertyReadready(BacnetPropertyNS::Identifier propertyId);
//    //! Returns the data associated with the propertyId.
//    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(Bacnet::ReadPropertyServiceData *rpStruct, Bacnet::Error *error);
//    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(BacnetPropertyNS::Identifier propId, quint32 arrayIdx, Bacnet::Error *error);
//    virtual int ensurePropertyReadySet(Bacnet::PropertyValue &writeData, Bacnet::Error *error);

//public://functions overriden from CovSupport
//    const QList<BacnetPropertyNS::Identifier> &covProperties();

//public://functions overridden from PropertyOwner
//    /** Hook function that is called after having requested reading/writting a property (which obviously doesn't
//        belong to Bacnet). Here we find, which property it is, add some BACnet specific parameters and propagate
//        them upwards to device, which calls Protocol handler.
//      */
//    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);

//    //! Hook fuction that is called when the property being observed changed
//    virtual void propertyValueChanged(Property *property);

//public:
//    QVariant::Type variantTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId);
//    Bacnet::BacnetDataInterface *createBacnetTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId, quint32 arrayIdx);

//    Bacnet::ObjectIdStruct _id;
//    QMap<BacnetPropertyNS::Identifier, Bacnet::BacnetDataInterface*> _specializedProperties;
//};

}

#endif // ANALOGINPUTOBJECT_H
