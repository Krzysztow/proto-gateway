#ifndef BACNETTSM_H
#define BACNETTSM_H


#include "bacnetaddress.h"
#include "bacnetcommon.h"


namespace Bacnet {

    class BacnetConfirmedServiceHandler;
    class AsynchronousBacnetTsmAction;
    class BacnetTSM
    {
    public:
        explicit BacnetTSM();

        bool sendAction(BacnetAddress &receiver, AsynchronousBacnetTsmAction *actionToSend);

        bool send(ObjectIdStruct &destinedObject, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);

//    public slots:
//        void generateResponse();

    private:
        BacnetConfirmedServiceHandler *tmp;
    };
}

#endif // BACNETTSM_H
