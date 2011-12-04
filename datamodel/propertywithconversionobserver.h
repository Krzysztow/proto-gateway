#ifndef DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H
#define DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H

#include "propertyobserver.h"

namespace DataModelNS {

class PropertyWithConversionObserver:
        public PropertyObserver
{
public:
    PropertyWithConversionObserver(PropertyOwner *container, PropertySubject *property);
};

} // namespace DataModel

#endif // DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H
