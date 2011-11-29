#include "externalpropertymapping.h"

using namespace Bacnet;

ExternalPropertyMapping::ExternalPropertyMapping(PropertySubject *mappedProperty, ExternalObjectReadStrategy *readAccessStrategy, ExternalObjectWriteStrategy *writeStrategy,
                                                 BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, Bacnet::ObjIdNum objectId):
    mappedProperty(mappedProperty),
    objectId(objectId),
    propertyId(propertyId),
    propertyArrayIdx(propertyArrayIdx),
    readAccessStrategy(readAccessStrategy),
    writeStrategy(writeStrategy)
{
}

bool ExternalPropertyMapping::isValid()
{
    return ( (0 != mappedProperty) &&
             (BacnetPropertyNS::UndefinedProperty != propertyId) &&
             (objectId != invalidObjIdNum()) );
}
