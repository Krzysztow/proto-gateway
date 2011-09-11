#ifndef BACNETTSM2_H
#define BACNETTSM2_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class BacnetConfirmedServiceHandler;
    class AsynchronousBacnetTsmAction;

    class BacnetTSM2:
            public QObject
    {
        Q_OBJECT
    public:
        explicit BacnetTSM2(QObject *parent = 0);

        bool sendAction(BacnetAddress &receiver, AsynchronousBacnetTsmAction *actionToSend);

        bool send(ObjectIdStruct &destinedObject, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);

    public:
        QList<BacnetConfirmedServiceHandler*> _asynchHandlers;

    signals:

    public slots:
        void generateResponse();

    };

}

#endif // BACNETTSM2_H
