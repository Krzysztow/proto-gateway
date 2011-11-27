#ifndef BACNETEXTERNALOBJECTS_H
#define BACNETEXTERNALOBJECTS_H

#include "bacnetcommon.h"
#include "property.h"

namespace Bacnet {

class BacnetExternalPropertyMapping
{
public:
    enum ReadAccessType {
        Access_COV,
        Access_COV_Uninitialized,
        Access_ReadRequest
    };

    BacnetExternalPropertyMapping(Property *internalProperty = 0, ReadAccessType readType = Access_ReadRequest,
                                  ObjIdNum objectIdentifier = invalidObjIdNum(), BacnetPropertyNS::Identifier propertyId = BacnetPropertyNS::UndefinedProperty, quint32 propertyArrayIdx = ArrayIndexNotPresent);

    bool isValid();

public:

    Property *property;
    ReadAccessType readType;
    ObjIdNum objectIdentifier;
    BacnetPropertyNS::Identifier propertyId;
    quint32 propertyArrayIdx;
};

}

#endif // BACNETEXTERNALOBJECTS_H
