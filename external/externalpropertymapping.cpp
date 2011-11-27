#include "externalpropertymapping.h"

using namespace Bacnet;

ExternalPropertyMapping::ExternalPropertyMapping(PropertySubject *mappedProperty, ExternalObjectReadStrategy *readAccessStrategy,
                                                 BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, ObjIdNum objectId):
    mappedProperty(mappedProperty),
    objectId(objectId),
    propertyId(propertyId),
    propertyArrayIdx(propertyArrayIdx),
    readAccessStrategy(readAccessStrategy)
{
}

bool ExternalPropertyMapping::isValid()
{
    return ( (0 != mappedProperty) &&
             (BacnetPropertyNS::UndefinedProperty != propertyId) &&
             (objectId != invalidObjIdNum()) );
}

int Bacnet::ExternalPropertyMapping::jobInterval_ms()
{
    return _jobIntervalMs;
}
