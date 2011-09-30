#ifndef CovNotificationRequestData_H
#define CovNotificationRequestData_H

#include <QtCore>

#include "bacnetservicedata.h"
#include "bacnetcommon.h"
#include "sequenceof.h"
#include "propertyvalue.h"

namespace Bacnet {
    class CovNotificationRequestData:
            public BacnetServiceData
    {
    public:
        CovNotificationRequestData();

        CovNotificationRequestData(quint8 subscrProcId, ObjectIdStruct initiatingObjectId, ObjectIdStruct monitoredObjectId,
                                   quint32 timeLeft = 0x00);

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        quint8 _subscribProcess;

        Bacnet::ObjectIdStruct _initiatingObjectId;
        Bacnet::ObjectIdStruct _monitoredObjectId;
        quint32 _timeLeft;
        SequenceOf<PropertyValue> _listOfValues;
    };
}

#endif // CovNotificationRequestData_H
