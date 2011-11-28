#include "discoverywrapper.h"

#include "externalconfirmedservicehandler.h"
#include "bacnetapplicationlayer.h"
#include "bacnetservicedata.h"

using namespace Bacnet;

DiscoveryWrapper::DiscoveryWrapper()
{
}

ConfirmedDiscoveryWrapper::ConfirmedDiscoveryWrapper(const ObjIdNum destinedObject, const BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend):
    _destinedObject(destinedObject),
    _sourceAddress(sourceAddress),
    _service(service),
    _serviceToSend(serviceToSend)
{
}

DiscoveryWrapper::Action ConfirmedDiscoveryWrapper::handleTimeout(BacnetApplicationLayerHandler *appLayer)
{
    Q_CHECK_PTR(appLayer);
    if (0 != _serviceToSend) {
        ExternalConfirmedServiceHandler::ActionToExecute todo = _serviceToSend->handleTimeout();
        if (ExternalConfirmedServiceHandler::ResendService == todo) {
            if (0 != appLayer) {
                appLayer->discover(_destinedObject);
                return LeaveMeInQueue;
            } else {
                qDebug("%s : app layer not provided!", __PRETTY_FUNCTION__);
                delete _serviceToSend;
                _serviceToSend = 0;
                return DeleteMe;
                Q_ASSERT(false);
            }
        } else {
            return DeleteMe;
        }
    } else {
        return DeleteMe;
    }
}

void ConfirmedDiscoveryWrapper::discoveryFinished(BacnetApplicationLayerHandler *appLayer, BacnetAddress &responderAddress)
{
    Q_CHECK_PTR(appLayer);
    appLayer->send(responderAddress, _sourceAddress, _service, _serviceToSend);
}

UnconfirmedDiscoveryWrapper::UnconfirmedDiscoveryWrapper(const ObjIdNum destinedObject, const BacnetAddress &source, BacnetServiceData *data, quint8 serviceChoice, int retryCount):
    _destinedObject(destinedObject),
    _source(source),
    _data(data),
    _serviceChoice(serviceChoice),
    _retryCount(retryCount)
{
}

DiscoveryWrapper::Action  UnconfirmedDiscoveryWrapper::handleTimeout(BacnetApplicationLayerHandler *appLayer)
{
    Q_CHECK_PTR(appLayer);
    Q_UNUSED(appLayer);
    --_retryCount;
    Q_ASSERT(_retryCount >= 0);
    if (_retryCount > 0) {
        return LeaveMeInQueue;
        appLayer->discover(_destinedObject);
    } else {
        return DeleteMe;
    }
}

void UnconfirmedDiscoveryWrapper::discoveryFinished(BacnetApplicationLayerHandler *appLayer, BacnetAddress &responderAddress)
{
    Q_CHECK_PTR(appLayer);
    appLayer->sendUnconfirmed(responderAddress, _source, *_data, _serviceChoice);
    delete _data;
}


