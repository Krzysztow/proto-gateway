#include "bacnetobject.h"

#include "property.h"
#include "bacnetdeviceobject.h"
#include "bacnetaddress.h"
#include "subscribecovservicedata.h"
#include "error.h"
#include "bacnetproperty.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

BacnetObject::BacnetObject(ObjectIdentifier &id, BacnetDeviceObject *parentDevice):
    _id(id),
    _parentDevice(parentDevice)
{
}

BacnetObject::BacnetObject(BacnetObjectTypeNS::ObjectType objectType, quint32 instanceNumber, BacnetDeviceObject *parentDevice):
    _id(objectType, instanceNumber),
    _parentDevice(parentDevice)
{
}

BacnetObject::~BacnetObject()
{
}

const ObjectIdentifier &BacnetObject::objectId() const
{
    return _id;
}

quint32 BacnetObject::objectIdNum() const
{
    return _id.objectIdNum();
}

void BacnetObject::setObjectName(QString name)
{
    _name = name;
}

QString BacnetObject::objectName() const
{
    //! \todo If there are performance problems/fragmentation issues - let's make it static QString - good if objectName is not kept for
    //! long time. Otherwise another instance call would change the value.
    if (_name.isEmpty())
        return QString("SNGBacnet:%1").arg(objectIdNum());
    return _name;
}

int BacnetObject::propertyReadTry(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Error *error)
{
    BacnetProperty *prop(0);
    prop = _properties[propertyId];
    if (0 == prop)
        prop = BacnetDefaultObject::instance()->defaultProperties(_id.type())[propertyId];

    if (0 != prop)
        return prop->getValue(data, propertyArrayIdx, error, false);

    //can't find the property!
    if (0 != error)
        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
    return Property::UnknownError;
}

BacnetDataInterfaceShared BacnetObject::propertyReadInstantly(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Error *error)
{
    BacnetProperty *prop(0);
    prop = _properties[propertyId];
    if (0 == prop)
        prop = BacnetDefaultObject::instance()->defaultProperties(_id.type())[propertyId];

    BacnetDataInterfaceShared data;
    if (0 != prop) {
        int ret = prop->getValue(data, propertyArrayIdx, error, true);
        if ( (ret != Property::ResultOk) || (data.isNull()) ) {
            qDebug("%s : can't read property (id, idx) = (%d , %d)!", __PRETTY_FUNCTION__, propertyId, propertyArrayIdx);
            if ( (0 != error) && (!error->hasError()) )
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
            data.clear();//clear it just in case!
        }
        return data;
    }

    //can't find the property!
    if (0 != error)
        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
    return data;
}

int BacnetObject::propertySet(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Error *error)
{
    BacnetProperty *prop(0);
    prop = _properties[propertyId];

    //don't look over default properties - those are not supposed to be changed!
    if (0 != prop)
        return prop->setValue(data, propertyArrayIdx, error);

    //can't find the property!
    if (0 != error)
        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
    return Property::UnknownError;
}

bool BacnetObject::addProperty(BacnetPropertyNS::Identifier identifier, BacnetProperty *property)
{
    Q_ASSERT(!_properties.contains(identifier));
    if (_properties.contains(identifier))
        return false;

    //we may insert it
    _properties.insert(identifier, property);
    return true;
}

const QMap<BacnetPropertyNS::Identifier, BacnetProperty *> & BacnetObject::objProperties() const
{
    return _properties;
}

BacnetProperty * BacnetObject::takeProperty(BacnetPropertyNS::Identifier identifier)
{
    return _properties[identifier];
}

//QList<PropertyValue*> BacnetObject::readCovValuesList()
//{
//    return QList<PropertyValue*>();
//}


