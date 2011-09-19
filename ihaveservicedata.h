#ifndef IHAVESERVICEDATA_H
#define IHAVESERVICEDATA_H

#include "bacnetservicedata.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class IHaveServiceData:
            public BacnetServiceData
    {
    public:
        IHaveServiceData();
        IHaveServiceData(ObjectIdStruct &devId, ObjectIdStruct &objId, QString objName);

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        ObjectIdStruct _devId;
        ObjectIdStruct _objId;
        QString _objName;
    };

}

#endif // IHAVESERVICEDATA_H
