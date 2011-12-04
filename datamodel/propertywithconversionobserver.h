#ifndef DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H
#define DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H

#include "property.h"

namespace DataModelNS {

class PropertyWithConversionObserver:
        public Property
{
public:
    PropertyWithConversionObserver();
};

} // namespace DataModel

#endif // DATAMODEL_PROPERTYWITHCONVERSIONOBSERVER_H
