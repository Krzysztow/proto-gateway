#ifndef BACNET_EXTERNALPROPERTYMAPPING_H
#define BACNET_EXTERNALPROPERTYMAPPING_H

#include "bacnetcommon.h"

class PropertySubject;

namespace Bacnet {

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
    ExternalPropertyMapping(PropertySubject *mappedProperty = 0, ExternalPropertyMapping::ReadAccessType readAccessType = ReadRP,
                            BacnetPropertyNS::Identifier propertyId = BacnetPropertyNS::UndefinedProperty, quint32 propertyArrayIdx = ArrayIndexNotPresent,
                            ObjIdNum objectId = invalidObjIdNum());

    bool isValid();
    int jobInterval_ms();

public:
    PropertySubject *mappedProperty;
    ObjIdNum objectId;
    BacnetPropertyNS::Identifier propertyId;
    quint32 propertyArrayIdx;
    ExternalPropertyMapping::ReadAccessType readAccessType;
    int _jobIntervalMs;
};



} // namespace Bacnet

#endif // BACNET_EXTERNALPROPERTYMAPPING_H
