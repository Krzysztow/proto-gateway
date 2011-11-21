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
    _parentDevice->addBacnetObject(this);
}

BacnetObject::BacnetObject(BacnetObjectTypeNS::ObjectType objectType, quint32 instanceNumber, BacnetDeviceObject *parentDevice):
    _id(objectType, instanceNumber),
    _parentDevice(parentDevice)
{
    _parentDevice->addBacnetObject(this);
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

bool Bacnet::BacnetObject::readClassDataHelper(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Bacnet::BacnetDataInterfaceShared &data, Bacnet::Error *error)
{
    Q_UNUSED(propertyArrayIdx);
    Q_UNUSED(error);

    switch (propertyId)
    {
    case (BacnetPropertyNS::ObjectIdentifier):
        data = BacnetDataInterfaceShared(new ObjectIdentifier(_id));
        return true;
    case (BacnetPropertyNS::ObjectType):
        data = BacnetDataInterfaceShared(new UnsignedInteger(_id.type()));
        return true;
    case (BacnetPropertyNS::ObjectName): {
        QString name = objectName();
        data = BacnetDataInterfaceShared(new CharacterString(name));
        return true;
    }
    case (BacnetPropertyNS::CovIncrement): {
        CovRealIcnrementHandler *covHndlr = covIncrementHandler(BacnetPropertyNS::PresentValue);
        if (0 != covHndlr) {
            data = BacnetDataInterfaceShared(covHndlr->createValue());
            return true;
        }
    }
    default:
        return false;
    }
}

int BacnetObject::propertyReadTry(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Error *error)
{
    if (readClassDataHelper(propertyId, propertyArrayIdx, data, error))
        return Property::ResultOk;

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
    BacnetDataInterfaceShared data;
    if (readClassDataHelper(propertyId, propertyArrayIdx, data, error))
        return data;

    BacnetProperty *prop(0);
    prop = _properties[propertyId];
    if (0 == prop)
        prop = BacnetDefaultObject::instance()->defaultProperties(_id.type())[propertyId];

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

void Bacnet::BacnetObject::propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                                        BacnetProperty *property, ArrayProperty *arrayProperty,
                                                        BacnetObject *parentObject, BacnetDeviceObject *deviceObject)
{
    Q_ASSERT(0 == parentObject);
    Q_ASSERT(0 == deviceObject);
    Q_CHECK_PTR(property);
    Q_CHECK_PTR(_parentDevice);
    Q_ASSERT( (arrayProperty == 0) || (_properties.values().contains(arrayProperty)) );
    Q_ASSERT( (arrayProperty != 0) || (_properties.values().contains(property)) );
    Q_UNUSED(deviceObject);

#warning "We should take care of information that value got changed!"
    if (0 != _parentDevice)
        _parentDevice->propertyAsynchActionFinished(asynchId, result, property, arrayProperty, this);
}

void Bacnet::BacnetObject::propertyValueChanged(Bacnet::BacnetProperty *property, Bacnet::ArrayProperty *propertyArray, Bacnet::BacnetObject *parentObject, Bacnet::BacnetDeviceObject *deviceObject)
{
    Q_ASSERT(0 == parentObject);
    Q_UNUSED(parentObject);
    Q_ASSERT(0 == deviceObject);
    Q_UNUSED(deviceObject);
    Q_CHECK_PTR(property);
    Q_CHECK_PTR(_parentDevice);
    //if property is in array (propertyArray != 0) then _properties contains propertyArray // this is equal to
    Q_ASSERT( (0 == propertyArray) || (propertyIdendifier(propertyArray) != BacnetPropertyNS::UndefinedProperty) );
    //if propertu is not an array (propertyArray == 0), then _propertyes contais prpoerty
    Q_ASSERT( (0 != propertyArray) || (propertyIdendifier(property) != BacnetPropertyNS::UndefinedProperty) );

    BacnetPropertyNS::Identifier propId(BacnetPropertyNS::UndefinedProperty);
    int propArrayIdx(ArrayIndexNotPresent);
    if (0 != propertyArray) {
        propArrayIdx = propertyArray->indexOfProperty(property);
        propId = propertyIdendifier(propertyArray);
        Q_ASSERT( (propId != BacnetPropertyNS::UndefinedProperty) && (propArrayIdx != ArrayIndexNotPresent) );
        if ( (BacnetPropertyNS::UndefinedProperty == propId) || (ArrayIndexNotPresent == propArrayIdx) ) {
            qDebug("%s : propId not foud (%d) or arrayIdx (%d)", __PRETTY_FUNCTION__, propId, propArrayIdx);
            return;
        }
    } else {
        propId = propertyIdendifier(property);
        Q_ASSERT(BacnetPropertyNS::UndefinedProperty != propId);
        if ( (BacnetPropertyNS::UndefinedProperty == propId) ) {
            qDebug("%s : propId not found", __PRETTY_FUNCTION__);
            return;
        }
    }

    //check if someone needs to be informed (is subscribed); if so, tell BacnetDevice to do so.
    propertyChanged(propId, propArrayIdx, this, _parentDevice);
}

BacnetPropertyNS::Identifier Bacnet::BacnetObject::propertyIdendifier(Bacnet::BacnetProperty *property) const
{
    QMap<BacnetPropertyNS::Identifier, BacnetProperty*>::const_iterator it = _properties.begin();
    QMap<BacnetPropertyNS::Identifier, BacnetProperty*>::const_iterator itEnd = _properties.end();
    for (; it != itEnd; ++it) {
        if (it.value() == property)
            return it.key();
    }

    return BacnetPropertyNS::UndefinedProperty;
}

const QList<BacnetPropertyNS::Identifier> Bacnet::BacnetObject::covProperties()
{
    return BacnetDefaultObject::covProperties(_id.type());
}

//QList<PropertyValue*> BacnetObject::readCovValuesList()
//{
//    return QList<PropertyValue*>();
//}


