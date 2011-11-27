#include "bacnetexternalpropertymapping.h"

using namespace Bacnet;

BacnetExternalPropertyMapping::BacnetExternalPropertyMapping(Property *internalProperty, ReadAccessType readType, ObjIdNum objectIdentifier, BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx):
    property(internalProperty),
    readType(readType),
    objectIdentifier(objectIdentifier),
    propertyId(propertyId),
    propertyArrayIdx(propertyArrayIdx)
{
}

bool Bacnet::BacnetExternalPropertyMapping::isValid()
{
    return ( (property != 0) &&
             (BacnetPropertyNS::UndefinedProperty != propertyId) &&
             (invalidObjIdNum() != objectIdentifier) );
}
