#ifndef BACNET_EXTERNALPROPERTYMAPPING_H
#define BACNET_EXTERNALPROPERTYMAPPING_H

#include "bacnetcommon.h"

class PropertySubject;

namespace Bacnet {

class ExternalObjectReadStrategy;
class ExternalObjectWriteStrategy;

class ExternalPropertyMapping
{
public:
    enum ReadAccessType {
        RadCov_Uninitialized,
        ReadCov_Confirmed,
        ReadCov_Unconfirmed,
        ReadCov_Uninitialized,
        ReadRP
    };

public:
    ExternalPropertyMapping(PropertySubject *mappedProperty, ExternalObjectReadStrategy *readAccessStrategy, ExternalObjectWriteStrategy *writeStrategy,
                            BacnetPropertyNS::Identifier propertyId = BacnetPropertyNS::UndefinedProperty, quint32 propertyArrayIdx = Bacnet::ArrayIndexNotPresent,
                            Bacnet::ObjIdNum objectId = Bacnet::invalidObjIdNum());

    bool isValid();

public:
    PropertySubject *mappedProperty;
    Bacnet::ObjIdNum objectId;
    BacnetPropertyNS::Identifier propertyId;
    quint32 propertyArrayIdx;
    ExternalObjectReadStrategy *readAccessStrategy;
    ExternalObjectWriteStrategy *writeStrategy;
};



} // namespace Bacnet

#endif // BACNET_EXTERNALPROPERTYMAPPING_H
