#ifndef IAMSERVICEDATA_H
#define IAMSERVICEDATA_H

#include "bacnetservicedata.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class IAmServiceData:
            public BacnetServiceData
    {
    public:
        IAmServiceData();
        IAmServiceData(ObjectIdStruct &objId, quint32 maxAPDULength, BacnetSegmentation segmentation, quint32 vendorId);

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        ObjectIdStruct _objId;
        quint32 _maxApduLength;
        BacnetSegmentation  _segmentationSupported;
        quint32 _vendorId;
    };

}
#endif // IAMSERVICEDATA_H
