#include "bacnetdeviceobject.h"

#include <QVariant>

#include "bacnetdefaultobject.h"
#include "bacnetprimitivedata.h"
#include "error.h"
#include "internalobjectshandler.h"
#include "readpropertyservicedata.h"
#include "propertyvalue.h"
#include "bacnetproperty.h"

#define WORKAROUND

using namespace Bacnet;

BacnetDeviceObject::BacnetDeviceObject(Bacnet::ObjectIdentifier &identifier, BacnetAddress &address):
    BacnetObject(identifier, this),
    _address(address)
{
#ifdef WORKAROUND
    _childObjects.insert(objectIdNum(), this);
#endif

    Q_ASSERT( identifier.type() == BacnetObjectTypeNS::Device);
    Q_ASSERT(_address.isAddrInitialized());
}

BacnetDeviceObject::BacnetDeviceObject(quint32 instanceNumber, BacnetAddress &address):
    BacnetObject(BacnetObjectTypeNS::Device, instanceNumber, this),
    _address(address)
{
#ifdef WORKAROUND
    _childObjects.insert(objectIdNum(), this);
#endif

    Q_ASSERT(instanceNumber <= 0x03fffff);
    Q_ASSERT(_address.isAddrInitialized());
}

BacnetDeviceObject::~BacnetDeviceObject()
{
    foreach (BacnetObject *child, _childObjects) {
        if (child != this)
            delete child;
    }
}

bool BacnetDeviceObject::readClassDataHelper(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Bacnet::BacnetDataInterfaceShared &data, Bacnet::Error *error)
{
    switch (propertyId)
    {
    case (BacnetPropertyNS::ActiveCovSubscriptions): {
        if (propertyArrayIdx != ArrayIndexNotPresent) {
            if (0 != error)
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodePropertyIsNotAnArray);
        } else {
            BacnetList *covList = new BacnetList();
            QList<CovSubscriptionShared>::Iterator it;
            QList<CovSubscriptionShared>::Iterator itEnd;
            foreach (BacnetObject *obj, _childObjects) {
                it = obj->covSubscriptions().begin();
                itEnd = obj->covSubscriptions().end();
                for (; it != itEnd; ++it) {
                    covList->value().append(*it);
                }
            }
            data = BacnetDataInterfaceShared(covList);
        }
        return true;
    }
    case (BacnetPropertyNS::ObjectList) : {
        BacnetList *devList = new BacnetList();
        foreach (BacnetObject *obj, _childObjects) {
            devList->value().append(BacnetDataInterfaceShared(new ObjectIdentifier(obj->objectIdNum())));
        }
        data = BacnetDataInterfaceShared(devList);
    }
        return true;
    case (BacnetPropertyNS::DeviceAddressBinding): {
        //! \note we return here an empty list. Most probably this would be really long if we didn't do so.
        data = BacnetDataInterfaceShared(new BacnetList());
    }
        return true;
    default:
        return false;
    }
}

//int BacnetDeviceObject::propertyReadTry(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx)
int BacnetDeviceObject::propertyReadTry(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Bacnet::Error *error)
{
    if (readClassDataHelper(propertyId, propertyArrayIdx, data, error))
        return ::Property::ResultOk;

    return BacnetObject::propertyReadTry(propertyId, propertyArrayIdx, data, error);
    //    //see if there are our cmd properties - this will be called the most often - thus first
    //    Property *propRequested = _cdmProperties[propertyId];
    //    if (0 != propRequested) {
    //        return propRequested->getValue(0);
    //    } else if (_specializedProperties.contains((propertyId)) ||//no our property - check if there are any overridern or default ones
    //               BacnetDefaultObject::instance()->defaultProperties(BacnetObjectType::Device).contains(propertyId)) {
    //        return Property::ResultOk;
    //    } else //not found - return false!
    //        return Property::UnknownError;
}

//void BacnetDeviceObject::propertyReadInstantly(BacnetPropertyNS::Identifier propId, quint32 arrayIdx, Bacnet::Error *error)
BacnetDataInterfaceShared BacnetDeviceObject::propertyReadInstantly(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Bacnet::Error *error)
{
    BacnetDataInterfaceShared data;
    if (readClassDataHelper(propertyId, propertyArrayIdx, data, error))
        return data;

    return BacnetObject::propertyReadInstantly(propertyId, propertyArrayIdx, error);
    //    Q_CHECK_PTR(error);
    //    //handle array case
    //    if (BacnetProperty::ObjectList == propId) {
    //        if (arrayIdx != Bacnet::ArrayIndexNotPresent) {
    //            Bacnet::BacnetArray *retArray = new Bacnet::BacnetArray();
    //            QMap<quint32, BacnetObject*>::iterator idIterator = _childObjects.begin();
    //            for (; idIterator != _childObjects.end(); ++idIterator) {
    //                retArray->addElement(new Bacnet::ObjectIdentifier(idIterator.value()->objectId()));
    //            }
    //            return retArray;
    //        } else {
    //            if (!(arrayIdx <= (uint)_childObjects.size())) {
    //                if (0 != error)
    //                    error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeInvalidArrayIndex);
    //                return 0;
    //            }
    //            return new Bacnet::ObjectIdentifier((_childObjects.begin() + (int)arrayIdx).value()->objectId());
    //        }
    //    }
    //#warning "NOT IMPLEMENTED - SENDING COV DEVICES"
    
    //    //handle cdm property
    //    Property *propRequested = _cdmProperties[propId];
    //    if (0 != propRequested) {
    //        QVariant::Type type = variantTypeForProperty_helper(propId);
    //        if (QVariant::Invalid == type) {
    //            qDebug("The property type %d is not translatable to internal type", type);
    //            if (0 != error)
    //                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
    //            return 0;
    //        }
    //        QVariant value(type);
    //        qint32 ret = propRequested->getValueInstant(&value);
    //        if ( (ret != Property::ResultOk) || (!value.isValid()) ) {
    //            qDebug("Invalid value when reading property (propId %d, object: %s, exp. type %d", propId, qPrintable(objectName()), type);
    //            if (0 != error)
    //                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
    //            return 0;
    //        }
    //        Bacnet::BacnetDataInterface *retProp = createBacnetTypeForProperty_helper(propId, Bacnet::ArrayIndexNotPresent);
    //        if ((0 == retProp) || !retProp->setInternal(value)) {
    //            delete retProp;
    //            if (0 != error)
    //                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
    //            qDebug("Cannot allocate/find BacnetData or set property (%d) with value %s", propId, qPrintable(value.toString()));
    //        }
    //        return retProp;
    //    }
    
    //    //handle overwritten and defaults
    //    Bacnet::BacnetDataInterface *existProp;
    //    existProp = _specializedProperties[propId];
    //    if (0 == existProp)
    //        existProp = BacnetDefaultObject::instance()->defaultProperties(BacnetObjectType::Device)[propId];
    //    if (0 != existProp)
    //        return new Bacnet::BacnetDataBaseDeletable(existProp);//make a proxy, so that proxy may be deleted as others and doesn't affect existProp.
    
    //    if (0 != error)
    //        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeUnknownProperty);
    //    return existProp;
}

//Bacnet::BacnetDataInterface *BacnetDeviceObject::propertyReadInstantly(Bacnet::ReadPropertyServiceData *rpStruct, Bacnet::Error *error)
//{
//    Q_CHECK_PTR(rpStruct);
//    return propertyReadInstantly(rpStruct->propertyId, rpStruct->arrayIndex, error);
//}

int BacnetDeviceObject::propertySet(BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, BacnetDataInterfaceShared &data, Bacnet::Error *error)
{
    //check some specific properties and if not met, delegate
    return BacnetObject::propertySet(propertyId, propertyArrayIdx, data, error);
    
    //    //assume one can set only cdm properties and those that are specialized.
    //    Q_CHECK_PTR(error);
    //    if (BacnetProperty::ObjectList == writeData._propertyId) {
    //        error->errorClass = BacnetErrorNS::ClassProperty;
    //        error->errorCode = BacnetErrorNS::CodeWriteAccessDenied;
    //        qDebug("ensurePropertyReadySet() : no reason to write to the childlist.");
    //        return Property::UnknownError;
    //    }
    
    //! \todo This is not so safe, since the asynchronous actions may reference this property - and we delete it.
    
    //    //handle cdm property
    //    Property *propToSet = _cdmProperties[writeData->propertyId];
    //    if (0 != propToSet) {
    //        QVariant::Type type = variantTypeForProperty_helper(rpStruct->propertyId);
    //        if (QVariant::Invalid == type) {
    //            qDebug("The property type %d is not translatable to internal type", type);
    //            error->errorClass = BacnetErrorNS::ClassProperty;
    //            error->errorCode = BacnetErrorNS::CodeOther;
    //            return 0;
    //        }
    //        QVariant value(type);
    //        qint32 ret = propToSet->getValueInstant(&value);
    //        if ( (ret != Property::ResultOk) || (!value.isValid()) ) {
    //            qDebug("Invalid value when reading property (propId %d, object: %s, exp. type %d", rpStruct->propertyId, qPrintable(objectName()), type);
    //            error->errorClass = BacnetErrorNS::ClassProperty;
    //            error->errorCode = BacnetErrorNS::CodeOther;
    //            return 0;
    //        }
    //        Bacnet::BacnetDataBase *retProp = createBacnetTypeForProperty_helper(rpStruct->propertyId);
    //        if ((0 == retProp) || !retProp->setInternal(value)) {
    //            delete retProp;
    //            error->errorClass = BacnetErrorNS::ClassProperty;
    //            error->errorCode = BacnetErrorNS::CodeOther;
    //            qDebug("Cannot allocate/find BacnetData or set property (%d) with value %s", rpStruct->propertyId, qPrintable(value.toString()));
    //        }
    //        return retProp;
    //    }
    
    //    return 0;
}

//const QList<BacnetPropertyNS::Identifier> &BacnetDeviceObject::covProperties()
//{
//    static QList<BacnetPropertyNS::Identifier> identifiers =
//            QList<BacnetPropertyNS::Identifier>() << BacnetPropertyNS::StatusFlags;
//    return identifiers;
//}

//QVariant::Type BacnetDeviceObject::variantTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId)
//{
//    switch (propertyId)
//    {
//    case (BacnetPropertyNS::SystemStatus):
//        return QVariant::UInt;
//    default:
//        return QVariant::Invalid;
//    }
//}

//Bacnet::BacnetDataInterface *BacnetDeviceObject::createBacnetTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId, quint32 arrayIdx)
//{
//    Q_UNUSED(arrayIdx);//the device object has no array-like properties.
//    switch (propertyId)
//    {
//    case (BacnetPropertyNS::SystemStatus):
//        return new Bacnet::UnsignedInteger();
//    default:
//        return 0;
//    }
//}

Bacnet::BacnetObject *BacnetDeviceObject::bacnetObject(quint32 instanceNumber)
{
    //first check if this is us
    //otherwise check children or return default value 0
    if (objectIdNum() == instanceNumber)
        return this;
    return _childObjects.value(instanceNumber);
}

bool BacnetDeviceObject::addBacnetObject(BacnetObject *object)
{
    Q_CHECK_PTR(object);

#ifdef WORKAROUND
    if (object == this)
        return true;//will be added later, in initialization.
#endif
    Q_ASSERT(!_childObjects.contains(object->objectIdNum()));
    if (!_childObjects.contains(object->objectIdNum())) {
        _childObjects.insert(object->objectIdNum(), object);
        return true;
    }
    return false;
}

//void BacnetDeviceObject::propertyIoFinished(int asynchId, int result, BacnetObject *object)
//{
//    //call the parent (Bacnet protocol handler) that we are done!
//    Q_CHECK_PTR(_handler);
//    _handler->propertyIoFinished(asynchId, result, object, this);
//}

void BacnetDeviceObject::propertyValueChanged(Bacnet::CovSubscription &subscriprion, BacnetObject *object, QList<Bacnet::PropertyValueShared> &propertiesValues)
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(_handler);
    _handler->propertyValueChanged(object, this, subscriprion, propertiesValues);
}

void BacnetDeviceObject::propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                                      BacnetProperty *property, ArrayProperty *arrayProperty,
                                                      BacnetObject *parentObject, BacnetDeviceObject *deviceObject)
{
    Q_CHECK_PTR(property);
    Q_CHECK_PTR(_handler);
    Q_ASSERT(0 == deviceObject);
    Q_UNUSED(deviceObject);

    Q_UNUSED(property);
    Q_UNUSED(arrayProperty);
    
    if (0 == parentObject) {//this means one of our properties called this function.
        //if property is in array (propertyArray != 0) then _properties contains propertyArray // this is equal to
        Q_ASSERT( (0 == arrayProperty) || (propertyIdendifier(arrayProperty) != BacnetPropertyNS::UndefinedProperty) );
        //if propertu is not an array (propertyArray == 0), then _propertyes contais prpoerty
        Q_ASSERT( (0 != arrayProperty) || (propertyIdendifier(property) != BacnetPropertyNS::UndefinedProperty) );
        parentObject = this;
    }
    
    _handler->propertyIoFinished(asynchId, result, parentObject, this);
}


//void BacnetDeviceObject::propertyValueChanged(BacnetProperty *property, ArrayProperty *propertyArray,
//                                              BacnetObject *parentObject, BacnetDeviceObject *deviceObject)
//{
//    Q_CHECK_PTR(property);
//    Q_CHECK_PTR(_handler);

//}

//void BacnetDeviceObject::propertyValueChanged(Property *property)
//{
//    Q_CHECK_PTR(property);
//    /** According to Table 13-1a, clause 13, we should check if the value is REAL (then, change should be reported when greater
//      than the service increment (if provided) or determined by device. We don't assume any determination of increment, so will
//      inform handler anytime internal property has changed.
//      */
////    BacnetPropertyNS::Identifier propId = findPropertyIdentifier(property);
////    Q_CHECK_PTR(_handler);
////    _handler->propertyValueChanged(this, this, propId);
//}

//void BacnetDeviceObject::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
//{
//    Q_CHECK_PTR(property);
//    Q_UNUSED(property);
//    this->propertyIoFinished(asynchId, actionResult, this);
//    //inform others that the property has changed!
//    propertyValueChanged(property);
//}

void BacnetDeviceObject::setHandler(InternalObjectsHandler *bHandler)
{
    _handler = bHandler;
}

const QMap<quint32, Bacnet::BacnetObject*> &BacnetDeviceObject::childObjects()
{
    return _childObjects;
}

BacnetAddress &BacnetDeviceObject::address()
{
    return _address;
}

//QVariant::Type BacnetDeviceObject::variantTypeForProperty_helper(BacnetPropertyNS::Identifier propertyId)
//{
//    switch (propertyId)
//    {
//    case (BacnetData::DeviceStatus)://fallthrough
//    case ():
//        return QVariant::UInt;
//    case ():
//        return QVariant::Int;
//    case ():
//        return QVariant::BitArray;
//    case ():
//        return QVariant::String;
//    case ():
//        return QVariant::Bool;
//    case ():
//        return QVariant::Double;
//    case ():
//        return QVariant::QByteArray;
//    case ():
//        return QVariant::QDate;
//    case ():
//        return QVariant::QTime;
//    default:
//        return QVariant::Invalid;
//    }
//}
