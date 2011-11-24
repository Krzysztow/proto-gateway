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
class BacnetApplicationLayerHandler;
}
class InternalObjectsHandler;
class BacnetDeviceObject;

namespace ServiceFactory
{
::InternalConfirmedRequestHandler *createConfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                          BacnetConfirmedRequestData *pciData,
                                                          Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                          Bacnet::BacnetApplicationLayerHandler *appLayer);

::InternalUnconfirmedRequestHandler *createUnconfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                              BacnetUnconfirmedRequestData &pciData,
                                                              Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                              Bacnet::BacnetApplicationLayerHandler *appLayer);
}

#endif // SERVICEFACTORY_H
