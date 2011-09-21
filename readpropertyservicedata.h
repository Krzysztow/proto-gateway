#ifndef READPROPERTYSERVICEDATA_H
#define READPROPERTYSERVICEDATA_H

#include "bacnetcommon.h"
#include "bacnetservicedata.h"

namespace Bacnet {

    class ReadPropertyServiceData:
            public BacnetServiceData
    {
    public:
        ReadPropertyServiceData();
        ReadPropertyServiceData(ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                                  quint32 arrayIndex = Bacnet::ArrayIndexNotPresent);

    public://overridden BacnetServiceData metho
        virtual qint32 fromRaw(quint8 *serviceData, quint16 bufferLength);
        virtual qint32 toRaw(quint8 *startPtr, quint16 bufferLength);

    public:
        Bacnet::ObjectIdStruct objId;
        BacnetProperty::Identifier propertyId;
        quint32 arrayIndex;
    };

}

#endif // READPROPERTYSERVICEDATA_H
