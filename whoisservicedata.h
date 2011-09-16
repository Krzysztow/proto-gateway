#ifndef WHOISSERVICEDATA_H
#define WHOISSERVICEDATA_H

#include <QtCore>

#include "bacnetservicedata.h"

namespace Bacnet {

    class WhoIsServiceData:
            public BacnetServiceData
    {
    public:
        WhoIsServiceData();

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public://is there any reason we should make it private?
        quint32 _rangeLowLimit;
        quint32 _rangeHighLimit;
    };

}
#endif // WHOISSERVICEDATA_H
