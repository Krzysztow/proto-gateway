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
        DeleteAll,
        DeleteWrapper,
        DontDelete
    };
    virtual quint32 handleTimeout(BacnetApplicationLayerHandler *appLayer, Action *action) = 0;
    virtual void deleteContents() = 0;
};

class ConfirmedDiscoveryWrapper:
        public DiscoveryWrapper
{
public:
    ConfirmedDiscoveryWrapper(const ObjectIdStruct &destinedObject, const BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend);

public://DiscoveryWrapper interface
    virtual quint32 handleTimeout(BacnetApplicationLayerHandler *appLayer, Action *action);
    virtual void deleteContents();

public:
    ObjectIdStruct _destinedObject;
    BacnetAddress _sourceAddress;
    BacnetServicesNS::BacnetConfirmedServiceChoice _service;
    ExternalConfirmedServiceHandler *_serviceToSend;
};

class UnconfirmedDiscoveryWrapper:
        public DiscoveryWrapper
{
public:
    UnconfirmedDiscoveryWrapper(const ObjectIdStruct &destinedObject, const BacnetAddress &source, BacnetServiceData *data, quint8 serviceChoice);

public://DiscoveryWrapper interface
    virtual quint32 handleTimeout(BacnetApplicationLayerHandler *appLayer, Action *action);
    virtual void deleteContents();

public:
    ObjectIdStruct _destinedObject;
    BacnetAddress _source;
    BacnetServiceData *_data;
    quint8 _serviceChoice;
};


} // namespace Bacnet

#endif // BACNET_DISCOVERYWRAPPER_H
