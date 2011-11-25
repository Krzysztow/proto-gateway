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
class ExternalObjectsHandler;
class BacnetApplicationLayerHandler;
}

namespace ServiceFactory
{
::InternalConfirmedRequestHandler *createConfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                          BacnetConfirmedRequestData *pciData,
                                                          Bacnet::BacnetDeviceObject *device,
                                                          Bacnet::BacnetApplicationLayerHandler *appLayer);

::InternalUnconfirmedRequestHandler *createUnconfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                              BacnetUnconfirmedRequestData &pciData,
                                                              Bacnet::BacnetDeviceObject *device,
                                                              Bacnet::BacnetApplicationLayerHandler *appLayer);
}

#endif // SERVICEFACTORY_H
