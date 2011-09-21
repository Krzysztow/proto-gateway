#ifndef BACNETTSM2_H
#define BACNETTSM2_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetcommon.h"
#include "bacnetservicedata.h"

namespace Bacnet {

    class BacnetConfirmedServiceHandler;
    class AsynchronousBacnetTsmAction;
    class Error;

    class BacnetTSM2:
            public QObject
    {
        Q_OBJECT
    public:
        explicit BacnetTSM2(QObject *parent = 0);

        bool sendAction(BacnetAddress &receiver, AsynchronousBacnetTsmAction *actionToSend);

        bool send(ObjectIdStruct &destinedObject, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);

        void sendAck(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData *data, quint8 invokeId, quint8 serviceChoice);
        void sendReject(BacnetAddress &destination, BacnetAddress &source, BacnetReject::RejectReason reason, quint8 invokeId);
        void sendError(BacnetAddress &destination, BacnetAddress &source, quint8 invokeId, BacnetServices::BacnetErrorChoice errorChoice, Error &error);
        void sendUnconfirmed(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);

    public:
        QList<BacnetConfirmedServiceHandler*> _asynchHandlers;

    signals:

    public slots:
        void generateResponse();

    };

}

#endif // BACNETTSM2_H
