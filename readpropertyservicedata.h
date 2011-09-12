#ifndef READPROPERTYSERVICEDATA_H
#define READPROPERTYSERVICEDATA_H

#include "bacnetcommon.h"

namespace Bacnet {

    class ReadPropertyServiceData
    {
    public:
        ReadPropertyServiceData();
        ReadPropertyServiceData(ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                                  quint32 arrayIndex = Bacnet::ArrayIndexNotPresent);

        qint32 fromRaw(quint8 *serviceData, quint8 buffLength);
        qint32 toRaw(quint8 *startPtr, quint8 buffLength);

        Bacnet::ObjectIdStruct objId;
        BacnetProperty::Identifier propertyId;
        quint32 arrayIndex;
    };

}

#endif // READPROPERTYSERVICEDATA_H
