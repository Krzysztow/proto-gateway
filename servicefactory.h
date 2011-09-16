#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include "bacnetcommon.h"

class BacnetService;
class InternalUnconfirmedRequestHandler;
class BacnetUnconfirmedRequestData;
namespace Bacnet {
    class BacnetTSM2;
    class ExternalObjectsHandler;
}
class InternalObjectsHandler;
class BacnetDeviceObject;

namespace ServiceFactory
{
    BacnetService *createService(quint8 *servicePtr, quint16 length,
                         quint8 serviceCode, qint32 *retCode);

    BacnetService *createBroadcastService(quint8 *servicePtr, quint16 length,
                                          quint8 serviceCode, qint32 *retCode);

    ::InternalUnconfirmedRequestHandler *createUnconfirmedHandler(BacnetUnconfirmedRequestData *pciData,
                                                                       Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                                       InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler);
};

#endif // SERVICEFACTORY_H
