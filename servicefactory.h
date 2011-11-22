#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include "bacnetcommon.h"

class InternalConfirmedRequestHandler;
class InternalUnconfirmedRequestHandler;
class BacnetUnconfirmedRequestData;
class BacnetConfirmedRequestData;
class BacnetAddress;

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
    ::InternalConfirmedRequestHandler *createConfirmedHandler(
                                                              BacnetAddress &requester, BacnetAddress &destination,
                                                              BacnetConfirmedRequestData *pciData,
                                                              Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                              InternalObjectsHandler *internalHandler);

//    BacnetService *createBroadcastService(quint8 *servicePtr, quint16 length,
//                                          quint8 serviceCode, qint32 *retCode);

::InternalUnconfirmedRequestHandler *createUnconfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                              BacnetUnconfirmedRequestData *pciData,
                                                              Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                              InternalObjectsHandler *internalHandler);
};

#endif // SERVICEFACTORY_H
