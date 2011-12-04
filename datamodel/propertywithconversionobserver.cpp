#include "propertywithconversionobserver.h"

using namespace DataModelNS;

PropertyWithConversionObserver::PropertyWithConversionObserver(PropertyOwner *container, PropertySubject *property):
    PropertyObserver(container, property)
{
}

//PropertyWithConversionObserver::~PropertyWithConversionObserver()
//{
//    _property->rmObserver(this);
//}

//void PropertyWithConversionObserver::setOwner(PropertyOwner *owner)
//{
//    _owner = owner;
//}

//PropertyOwner *PropertyWithConversionObserver::owner()
//{
//    return _owner;
//}

//int PropertyWithConversionObserver::getValue(QVariant *outValue)
//{
//    return _property->getValueSafe(outValue, this);
//}

//QVariant::Type PropertyWithConversionObserver::type()
//{
//    return _property->type();
//}

//int PropertyWithConversionObserver::setValue(QVariant &inValue)
//{
//    return _property->setValueSafe(inValue, this);
//}

//void PropertyWithConversionObserver::propertyValueChanged()
//{
//    _owner->propertyValueChanged(this);
//}

//int PropertyWithConversionObserver::getValueInstant(QVariant *outValue)
//{
//    Q_ASSERT(_property);
//    //we know it's not Property, but PropertySubject, so no virtual function is called, but straight PropertySubject one.
//    return _property->getValue(outValue);
//}

//void PropertyWithConversionObserver::asynchActionFinished(int asynchId, Property::ActiontResult actionResult)
//{
//    if (0 != _owner)
//        _owner->asynchActionFinished(asynchId, this, actionResult);
//}
