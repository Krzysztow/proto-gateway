#include "asynchronousbacnettsmaction.h"

#include "bacnetpci.h"
#include "bacnetservice.h"

using namespace Bacnet;

AsynchronousTsmResponseAction::AsynchronousTsmResponseAction(BacnetPciData *pciData,
                                                             BacnetService *service):
_pciData(pciData),
_service(service)
{
}

AsynchronousTsmResponseAction::~AsynchronousTsmResponseAction()
{
    delete _pciData;
    delete _service;
}

qint32 AsynchronousTsmResponseAction::toRaw(quint8 *dataStart, quint16 length)
{
    Q_CHECK_PTR(_pciData);
//    Q_CHECK_PTR(_service);//service pointer may be zero - that means we have no data after pci fields.

    quint8 *actualPtr(dataStart);
    qint32 ret;
    ret = _pciData->toRaw(actualPtr, length);
    if (ret < 0) return -1;
    actualPtr += ret;
    length -= ret;
    if (0 != _service) {//if no service data, it's fine.
        ret = _service->toRaw(actualPtr, length);
        if (ret < 0) return -2;
        actualPtr += ret;
    }
    return (actualPtr - dataStart);
}
