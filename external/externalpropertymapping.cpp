#include "externalpropertymapping.h"

using namespace Bacnet;

ExternalPropertyMapping::ExternalPropertyMapping(Property *mappedProperty, ExternalPropertyMapping::ReadAccessType type,
                                                 BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx, ObjIdNum objectId):
    mappedProperty(mappedProperty),
    objectId(objectId),
    propertyId(propertyId),
    propertyArrayIdx(propertyArrayIdx),
    readAccessType(type)
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
