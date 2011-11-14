#include "analoginputobject.h"

#include "property.h"
#include "bacnetdefaultobject.h"
#include "error.h"
#include "bacnetcommon.h"
#include "bacnetdeviceobject.h"
#include "readpropertyservicedata.h"
#include "propertyvalue.h"

#include <QSharedPointer>

using namespace Bacnet;

AnalogInputObject::AnalogInputObject(Bacnet::ObjectIdentifier &identifier, BacnetDeviceObject *parent):
    BacnetObject(identifier, parent)
{
    Q_ASSERT( identifier.type() == BacnetObjectType::AnalogInput);
    Q_CHECK_PTR(parent);
    parent->addBacnetObject(this);
}

AnalogInputObject::AnalogInputObject(quint32 instanceNumber, BacnetDeviceObject *parent):
    BacnetObject(BacnetObjectType::AnalogInput, instanceNumber, parent)
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

Bacnet::BacnetDataInterface *AnalogInputObject::propertyReadInstantly(BacnetProperty::Identifier propId, quint32 arrayIdx, Bacnet::Error *error)
{
    Q_CHECK_PTR(error);

    //handle cdm property
    Property *propRequested = _cdmProperties[propId];
    if (0 != propRequested) {
        QVariant::Type type = variantTypeForProperty_helper(propId);
        if (QVariant::Invalid == type) {
            qDebug("The property type %d is not translatable to internal type", type);
            error->setError(BacnetError::ClassProperty, BacnetError::CodeOther);
            return 0;
        }
        QVariant value(type);
        qint32 ret = propRequested->getValueInstant(&value);
        if ( (ret != Property::ResultOk) || (!value.isValid()) ) {
            qDebug("Invalid value when reading property (propId %d, object: %s, exp. type %d", propId, qPrintable(objectName()), type);
            error->setError(BacnetError::ClassProperty, BacnetError::CodeOther);
            return 0;
        }
        Bacnet::BacnetDataInterface *retProp = createBacnetTypeForProperty_helper(propId, Bacnet::ArrayIndexNotPresent);
        if ((0 == retProp) || !retProp->setInternal(value)) {
            delete retProp;
            error->setError(BacnetError::ClassProperty, BacnetError::CodeOther);
            qDebug("Cannot allocate/find BacnetData or set property (%d) with value %s", propId, qPrintable(value.toString()));
        }
        return retProp;
    }

    //handle overwritten and defaults
    Bacnet::BacnetDataInterface *existProp;
    existProp = _specializedProperties[propId];
    if (0 == existProp)
        existProp = BacnetDefaultObject::instance()->defaultProperties(BacnetObjectType::Device)[propId];
    if (0 != existProp)
        return new Bacnet::BacnetDataBaseDeletable(existProp);//make a proxy, so that proxy may be deleted as others and doesn't affect existProp.

    error->setError(BacnetError::ClassProperty, BacnetError::CodeUnknownProperty);
    return existProp;
}


//! Returns the data associated with the propertyId.
Bacnet::BacnetDataInterface *AnalogInputObject::propertyReadInstantly(ReadPropertyServiceData *rpStruct, Bacnet::Error *error)
{
    Q_CHECK_PTR(rpStruct);
    return propertyReadInstantly(rpStruct->propertyId, rpStruct->arrayIndex, error);
}

int AnalogInputObject::ensurePropertyReadySet(Bacnet::PropertyValue &writeData, Bacnet::Error *error)
{
    //assume one can set only cdm properties
    Q_CHECK_PTR(error);
    if (BacnetProperty::ObjectList == writeData._propertyId) {
        error->setError(BacnetError::ClassProperty, BacnetError::CodeWriteAccessDenied);
        qDebug("ensurePropertyReadySet() : no reason to write to the childlist.");
        return Property::UnknownError;
    }

    //handle cdm property
    Property *propToSet = _cdmProperties[writeData._propertyId];
    if (0 != propToSet) {
        QVariant value = writeData._value->toInternal();
        QVariant::Type type = variantTypeForProperty_helper(writeData._propertyId);
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

const QList<BacnetProperty::Identifier> &AnalogInputObject::covProperties()
{
    static QList<BacnetProperty::Identifier> identifiers =
            QList<BacnetProperty::Identifier>() << BacnetProperty::PresentValue << BacnetProperty::StatusFlags;
    return identifiers;
}

QVariant::Type AnalogInputObject::variantTypeForProperty_helper(BacnetProperty::Identifier propertyId)
{
    switch (propertyId)
    {
    case (BacnetProperty::PresentValue)://go through
    case (BacnetProperty::HighLimit):   //go through
    case (BacnetProperty::LowLimit):    //go through
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
    _parentDevice->propertyIoFinished(asynchId, actionResult, this);

    //this function is invoked when read or write succeeded - thus, check if we are supposed to inform COV subscribers
    propertyValueChanged(property);
}

/////////////////////////////////////////
//find property Id

//read proeprty - as BacnetDataInterface, created

//declare enum {NotChecked, Notify, DontNotify} isNotifyAll;

//foreach subscription
//  if subscription is for all properties
//      if isNotifyAll == NotChecked
//          check and set is NotifyAll to Notify or DontNotify
//      if isNotifyAll == Notify
//          for all CovProperties
//              readInstant covProperty and add it to the Sequence
//              if there is a problem inform, delete all propertues already created, exit loop
//          tell device to send notification with Sequence and CovSubscription






void AnalogInputObject::propertyValueChanged(Property *property)
{
    Q_CHECK_PTR(property);

    BacnetProperty::Identifier propId = findPropertyIdentifier(property);
    const quint32 propArrayIdx = ArrayIndexNotPresent;
    Q_ASSERT(propId != BacnetProperty::UndefinedProperty);


}
