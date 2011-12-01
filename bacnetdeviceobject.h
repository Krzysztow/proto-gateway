#ifndef BACNETDEVICEOBJECT_H
#define BACNETDEVICEOBJECT_H

#include <QtCore>

#include "bacnetobject.h"
#include "bacnetcommon.h"
#include "bacnetdata.h"
#include "bacnetobjectinternalsupport.h"
#include "propertyvalue.h"


namespace Bacnet {
class InternalObjectsHandler;

class BacnetDeviceObject:
        public BacnetObject
{
public:
    BacnetDeviceObject(Bacnet::ObjectIdentifier &identifier, BacnetAddress &address);
    BacnetDeviceObject(quint32 instanceNumber, BacnetAddress &address);
    ~BacnetDeviceObject();

public://overridden from BacnetObject
    virtual int propertyReadTry(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Bacnet::Error *error = 0);
    virtual BacnetDataInterfaceShared propertyReadInstantly(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Bacnet::Error *error = 0);
    virtual int propertySet(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Bacnet::Error *error = 0);

//    virtual const QList<BacnetPropertyNS::Identifier> &covProperties();

public://method overriden from InternalPropertyContainerSupport. Interface derived from BacnetObject. \todo I could extract BacnetObjectImpl from BacnetObject and make BacnetObjectImpl as well as DeviceObject inherit InternalPropertyContainerSupport interface.
    virtual void propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                              BacnetProperty *property = 0, ArrayProperty *arrayProperty = 0,
                                              BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0);

    //default BacnetObject::propertyValueChanged implementaiton is fine.
    //virtual void propertyValueChanged(BacnetProperty *property = 0, ArrayProperty *propertyArray = 0,
    //                                   BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0);

public://functions specific to BACnet device
    BacnetAddress &address();

    bool addBacnetObject(BacnetObject *object);
    BacnetObject *bacnetObject(quint32 instanceNumber);

    void setHandler(InternalObjectsHandler *bHandler);
    const QMap<quint32, BacnetObject*> &childObjects();
    Bacnet::BacnetDataInterface *constProperty(BacnetPropertyNS::Identifier propertyId);
    void propertyValueChanged(Bacnet::CovSubscription &subscriprion, BacnetObject *object, QList<Bacnet::PropertyValueShared> &propertiesValues);

private:
    bool readClassDataHelper(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Bacnet::BacnetDataInterfaceShared &data, Bacnet::Error *error);
//    QVariant::Type variantTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId);
//    Bacnet::BacnetDataInterface *createBacnetTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId, quint32 arrayIdx);

    /*public for a while
private:*/
public:
    Bacnet::ObjectIdStruct _id;
    QMap<quint32, BacnetObject*> _childObjects;
    BacnetAddress _address;
    InternalObjectsHandler *_handler;
};

}

#endif // BACNETDEVICEOBJECT_H
