#ifndef BACNET_DISCOVERYWRAPPER_H
#define BACNET_DISCOVERYWRAPPER_H

#include "bacnetcommon.h"
#include "bacnetaddress.h"

namespace Bacnet {

class ExternalConfirmedServiceHandler;
class BacnetServiceData;
class BacnetApplicationLayerHandler;

class DiscoveryWrapper
{
public:
    DiscoveryWrapper();

    enum Action {
        LeaveMeInQueue,
        DeleteMe
    };
    virtual Action handleTimeout(BacnetApplicationLayerHandler *appLayer) = 0;
    virtual void discoveryFinished(BacnetApplicationLayerHandler *appLayer, BacnetAddress &responderAddress) = 0;
};

class ConfirmedDiscoveryWrapper:
        public DiscoveryWrapper
{
public:
    ConfirmedDiscoveryWrapper(const ObjIdNum destinedObject, const BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend);

public://DiscoveryWrapper interface
    virtual Action handleTimeout(BacnetApplicationLayerHandler *appLayer);
    virtual void discoveryFinished(BacnetApplicationLayerHandler *appLayer, BacnetAddress &responderAddress);

public:
    ObjIdNum _destinedObject;
    BacnetAddress _sourceAddress;
    ExternalConfirmedServiceHandler *_serviceToSend;
};

class UnconfirmedDiscoveryWrapper:
        public DiscoveryWrapper
{
public:
    UnconfirmedDiscoveryWrapper(const ObjIdNum destinedObject, const BacnetAddress &source, BacnetServiceData *data, quint8 serviceChoice, int retryCount = 3);

public://DiscoveryWrapper interface
    virtual Action handleTimeout(BacnetApplicationLayerHandler *appLayer);
    virtual void discoveryFinished(BacnetApplicationLayerHandler *appLayer, BacnetAddress &responderAddress);

public:
    ObjIdNum _destinedObject;
    BacnetAddress _source;
    BacnetServiceData *_data;
    quint8 _serviceChoice;
    int _retryCount;
};


} // namespace Bacnet

#endif // BACNET_DISCOVERYWRAPPER_H
