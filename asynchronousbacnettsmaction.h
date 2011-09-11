#ifndef ASYNCHRONOUSBACNETTSMACTION_H
#define ASYNCHRONOUSBACNETTSMACTION_H

#include "bacnetaddress.h"

class BacnetService;
class BacnetPciData;

namespace Bacnet {

    class AsynchronousBacnetTsmAction
    {
    public:
        virtual ~AsynchronousBacnetTsmAction(){}

        virtual qint32 toRaw(quint8 *dataStart, quint16 length) = 0;
    };

    class AsynchronousTsmResponseAction:
            public AsynchronousBacnetTsmAction
    {
    public:
        AsynchronousTsmResponseAction(BacnetPciData *pciData, BacnetService *service);

    public://overwritten from AsynchronousBacnetTsmAction
        virtual ~AsynchronousTsmResponseAction();
        virtual qint32 toRaw(quint8 *dataStart, quint16 length);

    private:
        BacnetPciData *_pciData;
        BacnetService *_service;
    };

}
#endif // ASYNCHRONOUSBACNETTSMACTION_H
