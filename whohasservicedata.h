#ifndef WHOHASSERVICEDATA_H
#define WHOHASSERVICEDATA_H

#include <QtCore>

#include "bacnetprimitivedata.h"
#include "bacnetservicedata.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class WhoHasServiceData:
            public BacnetServiceData
    {
    public:
        WhoHasServiceData();
        WhoHasServiceData(quint32 objectIdNum, quint32 rangeLowLimit = Bacnet::InvalidInstanceNumber, quint32 rangeHighLimit = Bacnet::InvalidInstanceNumber);
        WhoHasServiceData(QString &objectName, quint32 rangeLowLimit = Bacnet::InvalidInstanceNumber, quint32 rangeHighLimit = Bacnet::InvalidInstanceNumber);

        ~WhoHasServiceData();

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public://is there any reason we should make it private?
        quint32 _rangeLowLimit;
        quint32 _rangeHighLimit;
        ObjectIdentifier *_objidentifier;
        CharacterString *_objName;
    };

}

#endif // WHOHASSERVICEDATA_H
