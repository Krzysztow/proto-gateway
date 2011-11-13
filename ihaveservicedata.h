#ifndef IHAVESERVICEDATA_H
#define IHAVESERVICEDATA_H

#include "bacnetservicedata.h"
#include "bacnetcommon.h"
#include "bacnetprimitivedata.h"

namespace Bacnet {

    class IHaveServiceData:
            public BacnetServiceData
    {
    public:
        IHaveServiceData();
        IHaveServiceData(ObjectIdentifier &devId, ObjectIdentifier &objId, QString objName);

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        ObjectIdentifier _devId;
        ObjectIdentifier _objId;
        QString _objName;
    };

}

#endif // IHAVESERVICEDATA_H
