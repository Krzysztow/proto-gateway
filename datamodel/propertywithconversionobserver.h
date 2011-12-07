#ifndef DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H
#define DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H

#include "propertyobserver.h"

namespace DataModelNS {

class PropertyConverter;

class PropertyWithConversionObserver:
        public PropertyObserver
{
public:
    //! If converte is 0, then actions for \sa PropertyUniversalConverter are used as default.
    PropertyWithConversionObserver(PropertyOwner *container, PropertySubject *property, QVariant::Type unconvertedType, PropertyConverter *converter = 0);

public://overriden methods of PropertyObserver
    virtual int getValue(QVariant *outValue);
    virtual int setValue(QVariant &inValue);
    virtual int getValueInstant(QVariant *outValue);


    //! Property::type() implamented. Returns type of the property before conversion to subjects property.
    QVariant::Type type();

public:
    QVariant::Type subjectProperty();

private:
    PropertyConverter *myConverter();

private:
    PropertyConverter *_converter;
    QVariant::Type _unconvertedType;
};

} // namespace DataModel

#endif // DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H
