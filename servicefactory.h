#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include "bacnetcommon.h"

class InternalConfirmedRequestHandler;
class InternalUnconfirmedRequestHandler;
class BacnetUnconfirmedRequestData;
class BacnetConfirmedRequestData;
namespace Bacnet {
    class BacnetDeviceObject;
    class BacnetServiceData;
    class BacnetTSM2;
    class ExternalObjectsHandler;
}
class InternalObjectsHandler;
class BacnetDeviceObject;

namespace ServiceFactory
{
    ::InternalConfirmedRequestHandler *createConfirmedHandler(BacnetConfirmedRequestData *pciData,
                                                              Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                              InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler);

//    BacnetService *createBroadcastService(quint8 *servicePtr, quint16 length,
//                                          quint8 serviceCode, qint32 *retCode);

    ::InternalUnconfirmedRequestHandler *createUnconfirmedHandler(BacnetUnconfirmedRequestData *pciData,
                                                                       Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                                       InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler);
};

#endif // SERVICEFACTORY_H
