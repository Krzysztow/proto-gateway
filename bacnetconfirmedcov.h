#ifndef BACNETCONFIRMEDCOV_H
#define BACNETCONFIRMEDCOV_H

#include <QtCore>

#include "bacnetservice.h"
#include "bacnetcommon.h"
#include "sequenceof.h"
#include "propertyvalue.h"

namespace Bacnet {

    class BacnetConfirmedCOV:
            public BacnetService
    {
    public:
        BacnetConfirmedCOV();
        ~BacnetConfirmedCOV();

        qint16 parseFromRaw(quint8 *servicePtr, quint16 length);

    private:
        quint8 _subscribProcess;

        Bacnet::ObjectIdStruct _initiatingDevObjtId;
        Bacnet::ObjectIdStruct _monitoredObjectId;
        quint32 _timeLeft;
        SequenceOf<PropertyValue> _listOfValues;
    };

}

#endif // BACNETCONFIRMEDCOV_H
