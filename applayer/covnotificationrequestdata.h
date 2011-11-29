#ifndef COVNOTIFICATIONREQUESTDATA_H
#define COVNOTIFICATIONREQUESTDATA_H

#include <QtCore>

#include "bacnetservicedata.h"
#include "bacnetcommon.h"
#include "sequenceof.h"
#include "bacnetprimitivedata.h"
#include "propertyvalue.h" //this has to be included for SequenceOf template.

namespace Bacnet {

    class CovNotificationRequestData:
            public BacnetServiceData
    {
    public:
        CovNotificationRequestData();

        CovNotificationRequestData(quint8 subscrProcId, const ObjectIdentifier &initiatingObjectId, const ObjectIdentifier &monitoredObjectId,
                                   quint32 timeLeft = 0x00);

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        quint8 _subscribProcess;

        ObjectIdentifier _initiatingDevObjtId;
        ObjectIdentifier _monitoredObjectId;
        quint32 _timeLeft;
        SequenceOf<PropertyValue> _listOfValues;
    };
}

#endif // COVNOTIFICATIONREQUESTDATA_H
