#ifndef BACNETEXTERNALOBJECTS_H
#define BACNETEXTERNALOBJECTS_H

#include "bacnetcommon.h"
#include "property.h"

namespace Bacnet {

    class BacnetExternalObjects
    {
    public:
        BacnetExternalObjects();

        enum ReadAccessType {
            Access_COV,
            Access_COV_Uninitialized,
            Access_ReadRequest
        };

        struct ExternalRoutingElement {
            Property *_property;
            quint32 _deviceIdentifier;
            quint32 _objectIdentifier;
            BacnetProperty::Identifier _propertyId;
            quint32 _propertyArrayIdx;
            ReadAccessType _read;
        };
    };

}

#endif // BACNETEXTERNALOBJECTS_H
