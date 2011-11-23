#ifndef BACNETTSM_H
#define BACNETTSM_H


#include "bacnetaddress.h"
#include "bacnetcommon.h"


namespace Bacnet {

    class ExternalConfirmedServiceHandler;
    class AsynchronousBacnetTsmAction;
    class BacnetTSM
    {
    public:
        explicit BacnetTSM();

        bool sendAction(BacnetAddress &receiver, AsynchronousBacnetTsmAction *actionToSend);

        bool send(ObjectIdStruct &destinedObject, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);

//    public slots:
//        void generateResponse();

    private:
        ExternalConfirmedServiceHandler *tmp;
    };
}

#endif // BACNETTSM_H
