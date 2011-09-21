#ifndef WRITEPROPERTYSERVICEDATA_H
#define WRITEPROPERTYSERVICEDATA_H

#include "bacnetservicedata.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class WritePropertyServiceData:
            public BacnetServiceData
    {
    public:
        WritePropertyServiceData();
        WritePropertyServiceData(Bacnet::ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                                 Bacnet::BacnetDataInterface *writeValue, quint32 arrayIndex);
        virtual ~WritePropertyServiceData();

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        Bacnet::ObjectIdStruct _objectId;
        Bacnet::PropertyValueStruct _propValue;
    };

}

#endif // WRITEPROPERTYSERVICEDATA_H
