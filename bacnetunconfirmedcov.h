#ifndef BACNETUNCONFIRMEDCOV_H
#define BACNETUNCONFIRMEDCOV_H

#include <QtCore>

#include "bacnetservice.h"
#include "bacnetcommon.h"
#include "sequenceof.h"
#include "propertyvalue.h"

namespace Bacnet {

    class BacnetUnconfirmedCov:
            public BacnetService
    {
    public:
        BacnetUnconfirmedCov();
        ~BacnetUnconfirmedCov();

        qint16 parseFromRaw(quint8 *servicePtr, quint16 length);

    private:
        quint8 _subscribProcess;

        Bacnet::ObjectIdStruct _initiatingDevObjtId;
        Bacnet::ObjectIdStruct _monitoredObjectId;
        quint32 _timeLeft;
        SequenceOf<PropertyValue> _listOfValues;
    };

}
#endif // BACNETUNCONFIRMEDCOV_H
