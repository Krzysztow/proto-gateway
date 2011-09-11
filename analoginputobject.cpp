#include "analoginputobject.h"

#include "property.h"
#include "bacnetdefaultobject.h"
#include "error.h"
#include "bacnetcommon.h"
#include "bacnetdeviceobject.h"

using namespace Bacnet;

AnalogInputObject::AnalogInputObject(Bacnet::ObjectIdStruct identifier, BacnetDeviceObject *parent):
        BacnetObject(identifier),
        _parentDevice(parent)
{
    Q_ASSERT( (identifier.objectType) == BacnetObjectType::Device);
    Q_CHECK_PTR(parent);
    parent->addBacnetObject(this);
}

AnalogInputObject::AnalogInputObject(quint32 instanceNumber, BacnetDeviceObject *parent):
        BacnetObject(BacnetObjectType::AnalogInput, instanceNumber),
        _parentDevice(parent)
{
    Q_ASSERT(instanceNumber <= 0x03fffff);
    Q_CHECK_PTR(parent);
    parent->addBacnetObject(this);
}

AnalogInputObject::~AnalogInputObject()
{
    qDeleteAll(_specializedProperties);
}

//! Used to check if we can read from the device. If not yet, the asynchronous id for read request should be returned or error status.
int AnalogInputObject::ensurePropertyReadyRead(BacnetProperty::Identifier propertyId)
{
    //see if there are our cmd properties - this will be called the most often - thus first
    Property *propRequested = _cdmProperties[propertyId];
    if (0 != propRequested) {
        return propRequested->getValue(0);
    } else if (_specializedProperties.contains((propertyId)) ||//no our property - check if there are any overridern or default ones
               BacnetDefaultObject::instance()->defaultProperties(BacnetObjectType::AnalogInput).contains(propertyId)) {
        return Property::ResultOk;
    } else //not found - return false!
        return Property::UnknownError;
}

//! Returns the data associated with the propertyId.
Bacnet::BacnetDataInterface *AnalogInputObject::propertyReadInstantly(Bacnet::ReadPropertyStruct *rpStruct, Bacnet::Error *error)
{
    Q_CHECK_PTR(rpStruct);

    //handle cdm property
    Property *propRequested = _cdmProperties[rpStruct->propertyId];
    if (0 != propRequested) {
        QVariant::Type type = variantTypeForProperty_helper(rpStruct->propertyId);
        if (QVariant::Invalid == type) {
            qDebug("The property type %d is not translatable to internal type", type);
            error->setError(BacnetError::ClassProperty, BacnetError::CodeOther);
            return 0;
        }
        QVariant value(type);
        qint32 ret = propRequested->getValueInstant(&value);
        if ( (ret != Property::ResultOk) || (!value.isValid()) ) {
            qDebug("Invalid value when reading property (propId %d, object: %s, exp. type %d", rpStruct->propertyId, qPrintable(objectName()), type);
            error->setError(BacnetError::ClassProperty, BacnetError::CodeOther);
            return 0;
        }
        Bacnet::BacnetDataInterface *retProp = createBacnetTypeForProperty_helper(rpStruct->propertyId, Bacnet::ArrayIndexNotPresent);
        if ((0 == retProp) || !retProp->setInternal(value)) {
            delete retProp;
            error->setError(BacnetError::ClassProperty, BacnetError::CodeOther);
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

    error->setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    return existProp;
}

int AnalogInputObject::ensurePropertyReadySet(Bacnet::PropertyValueStruct &writeData, Bacnet::Error *error)
{
    //assume one can set only cdm properties
    Q_CHECK_PTR(error);
    if (BacnetProperty::ObjectList == writeData.propertyId) {
        error->setError(BacnetError::ClassProperty, BacnetError::CodeWriteAccessDenied);
        qDebug("ensurePropertyReadySet() : no reason to write to the childlist.");
        return Property::UnknownError;
    }

    //handle cdm property
    Property *propToSet = _cdmProperties[writeData.propertyId];
    if (0 != propToSet) {
        QVariant value = writeData.value->toInternal();
        QVariant::Type type = variantTypeForProperty_helper(writeData.propertyId);
        if ((QVariant::Invalid == type) || !value.canConvert(type)) {
            qDebug("The property type %d is not translatable to internal type", type);
            error->setError(BacnetError::ClassProperty, BacnetError::CodeInvalidDataType);
            return Property::UnknownError;
        }
        return propToSet->setValue(value);
    }

    error->setError(BacnetError::ClassProperty, BacnetError::CodeWriteAccessDenied);
    return Property::UnknownError;
}

QVariant::Type AnalogInputObject::variantTypeForProperty_helper(BacnetProperty::Identifier propertyId)
{
    switch (propertyId)
    {
    case (BacnetProperty::PresentValue)://go through
    case (BacnetProperty::HighLimit):   //go through
    case (BacnetProperty::LowLimit):
    case (BacnetProperty::Deadband):
        return QVariant::Double;
    case (BacnetProperty::StatusFlags): //go through
    case (BacnetProperty::LimitEnable): //go through
    case (BacnetProperty::EventEnable):
        return QVariant::BitArray;
    case (BacnetProperty::EventState):  //go through
    case (BacnetProperty::Reliability):
        return QVariant::UInt;//enumeration
    case (BacnetProperty::UpdateInterval):
        return QVariant::UInt;
    default:
        return QVariant::Invalid;
    }
}

Bacnet::BacnetDataInterface *AnalogInputObject::createBacnetTypeForProperty_helper(BacnetProperty::Identifier propertyId, quint32 arrayIdx)
{
    //what about created Abstract values! They shouldn't happed.
    //! \todo Maybe the corresponding part of createDataForObjectProperty() should be moved here, but then
    //we would have to bring all the data dependencies here.
    return BacnetDefaultObject::createDataForObjectProperty(BacnetObjectType::AnalogInput, propertyId, arrayIdx);
}


//////////////////////////////////////////////////////////////////////
////////////////////////////PropertyOwner/////////////////////////////
//////////////////////////////////////////////////////////////////////

void AnalogInputObject::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_CHECK_PTR(property);
    Q_CHECK_PTR(_parentDevice);
//    getDataAndInformDevice(this, _parentDevice, asynchId, actionResult, property);
    _parentDevice->propertyChanged(asynchId, actionResult, this);
}

int AnalogInputObject::getPropertyRequest(PropertySubject *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    Q_UNUSED(toBeGotten);
    return Property::UnknownError;
}

int AnalogInputObject::setPropertyRequest(PropertySubject *toBeSet, QVariant &value)
{
    Q_CHECK_PTR(toBeSet);
    Q_UNUSED(toBeSet);
    Q_UNUSED(value);
    return Property::UnknownError;
}
