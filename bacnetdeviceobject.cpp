#include "bacnetdeviceobject.h"

#include <QVariant>

#include "bacnetdefaultobject.h"
#include "bacnetprimitivedata.h"
#include "error.h"
#include "asynchsetter.h"
#include "readpropertyservicedata.h"

using namespace Bacnet;

BacnetDeviceObject::BacnetDeviceObject(Bacnet::ObjectIdStruct identifier):
        BacnetObject(identifier)
{
    Q_ASSERT( (identifier.objectType) == BacnetObjectType::Device);
}

BacnetDeviceObject::BacnetDeviceObject(quint32 instanceNumber):
        BacnetObject(BacnetObjectType::Device, instanceNumber)
{
    Q_ASSERT(instanceNumber <= 0x03fffff);
}

BacnetDeviceObject::~BacnetDeviceObject()
{
    qDeleteAll(_specializedProperties);
    qDeleteAll(_childObjects);
}

int BacnetDeviceObject::ensurePropertyReadyRead(BacnetProperty::Identifier propertyId)
{
    //see if there are our cmd properties - this will be called the most often - thus first
    Property *propRequested = _cdmProperties[propertyId];
    if (0 != propRequested) {
        return propRequested->getValue(0);
    } else if (_specializedProperties.contains((propertyId)) ||//no our property - check if there are any overridern or default ones
               BacnetDefaultObject::instance()->defaultProperties(BacnetObjectType::Device).contains(propertyId)) {
        return Property::ResultOk;
    } else //not found - return false!
        return Property::UnknownError;
}

Bacnet::BacnetDataInterface *BacnetDeviceObject::propertyReadInstantly(Bacnet::ReadPropertyServiceData *rpStruct, Bacnet::Error *error)
{
    Q_CHECK_PTR(rpStruct);
    //handle array case
    if (BacnetProperty::ObjectList == rpStruct->propertyId) {
        if (rpStruct->arrayIndex != Bacnet::ArrayIndexNotPresent) {
            Bacnet::BacnetArray *retArray = new Bacnet::BacnetArray();
            QMap<quint32, BacnetObject*>::iterator idIterator = _childObjects.begin();
            for (; idIterator != _childObjects.end(); ++idIterator) {
                retArray->addElement(new Bacnet::ObjectIdentifier(idIterator.value()->objectId()));
            }
            return retArray;
        } else {
            if (!(rpStruct->arrayIndex <= _childObjects.size())) {
                error->errorClass = BacnetError::ClassProperty;
                error->errorCode = BacnetError::CodeInvalidArrayIndex;
                return 0;
            }
            return new Bacnet::ObjectIdentifier((_childObjects.begin() + (int)rpStruct->arrayIndex).value()->objectId());
        }
    }


    //handle cdm property
    Property *propRequested = _cdmProperties[rpStruct->propertyId];
    if (0 != propRequested) {
        QVariant::Type type = variantTypeForProperty_helper(rpStruct->propertyId);
        if (QVariant::Invalid == type) {
            qDebug("The property type %d is not translatable to internal type", type);
            error->errorClass = BacnetError::ClassProperty;
            error->errorCode = BacnetError::CodeOther;
            return 0;
        }
        QVariant value(type);
        qint32 ret = propRequested->getValueInstant(&value);
        if ( (ret != Property::ResultOk) || (!value.isValid()) ) {
            qDebug("Invalid value when reading property (propId %d, object: %s, exp. type %d", rpStruct->propertyId, qPrintable(objectName()), type);
            error->errorClass = BacnetError::ClassProperty;
            error->errorCode = BacnetError::CodeOther;
            return 0;
        }
        Bacnet::BacnetDataInterface *retProp = createBacnetTypeForProperty_helper(rpStruct->propertyId, Bacnet::ArrayIndexNotPresent);
        if ((0 == retProp) || !retProp->setInternal(value)) {
            delete retProp;
            error->errorClass = BacnetError::ClassProperty;
            error->errorCode = BacnetError::CodeOther;
            qDebug("Cannot allocate/find BacnetData or set property (%d) with value %s", rpStruct->propertyId, qPrintable(value.toString()));
        }
        return retProp;
    }

    //handle overwritten and defaults
    Bacnet::BacnetDataInterface *existProp;
    existProp = _specializedProperties[rpStruct->propertyId];
    if (0 == existProp)
        existProp = BacnetDefaultObject::instance()->defaultProperties(BacnetObjectType::Device)[rpStruct->propertyId];
    if (0 != existProp)
        return new Bacnet::BacnetDataBaseDeletable(existProp);//make a proxy, so that proxy may be deleted as others and doesn't affect existProp.

    error->errorClass = BacnetError::ClassProperty;
    error->errorCode = BacnetError::CodeUnknownProperty;
    return existProp;
}

int BacnetDeviceObject::ensurePropertyReadySet(Bacnet::PropertyValueStruct &writeData, Bacnet::Error *error)
{
    //assume one can set only cdm properties
    Q_CHECK_PTR(error);
    if (BacnetProperty::ObjectList == writeData.propertyId) {
        error->errorClass = BacnetError::ClassProperty;
        error->errorCode = BacnetError::CodeWriteAccessDenied;
        qDebug("ensurePropertyReadySet() : no reason to write to the childlist.");
        return Property::UnknownError;
    }

//    //handle cdm property
//    Property *propToSet = _cdmProperties[writeData->propertyId];
//    if (0 != propToSet) {
//        QVariant::Type type = variantTypeForProperty_helper(rpStruct->propertyId);
//        if (QVariant::Invalid == type) {
//            qDebug("The property type %d is not translatable to internal type", type);
//            error->errorClass = BacnetError::ClassProperty;
//            error->errorCode = BacnetError::CodeOther;
//            return 0;
//        }
//        QVariant value(type);
//        qint32 ret = propToSet->getValueInstant(&value);
//        if ( (ret != Property::ResultOk) || (!value.isValid()) ) {
//            qDebug("Invalid value when reading property (propId %d, object: %s, exp. type %d", rpStruct->propertyId, qPrintable(objectName()), type);
//            error->errorClass = BacnetError::ClassProperty;
//            error->errorCode = BacnetError::CodeOther;
//            return 0;
//        }
//        Bacnet::BacnetDataBase *retProp = createBacnetTypeForProperty_helper(rpStruct->propertyId);
//        if ((0 == retProp) || !retProp->setInternal(value)) {
//            delete retProp;
//            error->errorClass = BacnetError::ClassProperty;
//            error->errorCode = BacnetError::CodeOther;
//            qDebug("Cannot allocate/find BacnetData or set property (%d) with value %s", rpStruct->propertyId, qPrintable(value.toString()));
//        }
//        return retProp;
//    }

    return 0;
}

QVariant::Type BacnetDeviceObject::variantTypeForProperty_helper(BacnetProperty::Identifier propertyId)
{
    switch (propertyId)
    {
    case (BacnetProperty::SystemStatus):
        return QVariant::UInt;
#warning "Remove this case - was here only for test purposes!"
    case (BacnetProperty::PresentValue):
        return QVariant::Double;
    default:
        return QVariant::Invalid;
    }
}

Bacnet::BacnetDataInterface *BacnetDeviceObject::createBacnetTypeForProperty_helper(BacnetProperty::Identifier propertyId, quint32 arrayIdx)
{
    switch (propertyId)
    {
    case (BacnetProperty::SystemStatus):
        return new Bacnet::UnsignedInteger();
    case (BacnetProperty::PresentValue):
        return new Bacnet::Real();
    default:
        return 0;
    }
}

BacnetObject *BacnetDeviceObject::bacnetObject(quint32 instanceNumber)
{
    //first check if this is us
    //otherwise check children or return default value 0
    if (objectIdNum() == instanceNumber)
        return this;
    return _childObjects[instanceNumber];
}

bool BacnetDeviceObject::addBacnetObject(BacnetObject *object)
{
    Q_CHECK_PTR(object);
    Q_ASSERT(!_childObjects.contains(object->objectIdNum()));
    if (!_childObjects.contains(object->objectIdNum())) {
        _childObjects.insert(object->objectIdNum(), object);
        return true;
    }
    return false;
}

void BacnetDeviceObject::propertyChanged(int asynchId, int result, BacnetObject *object)
{
    //call the parent (Bacnet protocol handler) that we are done!
    Q_CHECK_PTR(_handler);
    _handler->propertyIoFinished(asynchId, result, object, this);
}

int BacnetDeviceObject::getPropertyRequest(PropertySubject *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    Q_ASSERT(false);
    return Property::UnknownError;
}

int BacnetDeviceObject::setPropertyRequest(PropertySubject *toBeSet, QVariant &value)
{
    Q_CHECK_PTR(toBeSet);
    Q_ASSERT(false);
    return Property::UnknownError;
}

void BacnetDeviceObject::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_CHECK_PTR(property);
    Q_UNUSED(property);
    this->propertyChanged(asynchId, actionResult, this);
}

void BacnetDeviceObject::setHandler(AsynchSetter *bHandler)
{
    _handler = bHandler;
}


//QVariant::Type BacnetDeviceObject::variantTypeForProperty_helper(BacnetProperty::Identifier propertyId)
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
