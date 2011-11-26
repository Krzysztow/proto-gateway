#ifndef BACNETSERVICEDATA_H
#define BACNETSERVICEDATA_H

#include <QtCore>

namespace Bacnet {

    class BacnetServiceData
    {
    public:
        virtual qint32 fromRaw(quint8 *serviceData, quint16 bufferLength) = 0;
        virtual qint32 toRaw(quint8 *startPtr, quint16 bufferLength) = 0;
    };

}

#endif // BACNETSERVICEDATA_H
