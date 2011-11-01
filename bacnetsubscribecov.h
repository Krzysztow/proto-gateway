#ifndef BACNETSUBSCRIBECOV_H
#define BACNETSUBSCRIBECOV_H

#include <QtCore>

#include "bacnetservice.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class BacnetSubscribeCov:
            public BacnetService
    {
    public:
        BacnetSubscribeCov();
        ~BacnetSubscribeCov();

        qint16 parseFromRaw(quint8 *servicePtr, quint16 length);

    private:
        quint32 _subscrProcessId;
        ObjectIdStruct _monitoredObjId;
        bool _issueConfirmedNotifs;
        quint32 _lifetime;
    };

}

#endif // BACNETSUBSCRIBECOV_H
