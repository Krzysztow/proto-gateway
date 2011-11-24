#include "discoverywrapper.h"

#include "externalconfirmedservicehandler.h"
#include "bacnetapplicationlayer.h"

using namespace Bacnet;

DiscoveryWrapper::DiscoveryWrapper()
{
}

ConfirmedDiscoveryWrapper::ConfirmedDiscoveryWrapper(const ObjectIdStruct &destinedObject, const BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend):
    _destinedObject(destinedObject),
    _sourceAddress(sourceAddress),
    _service(_service),
    _serviceToSend(serviceToSend)
{
}

quint32 ConfirmedDiscoveryWrapper::handleTimeout(BacnetApplicationLayerHandler *appLayer, DiscoveryWrapper::Action *action)
{
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(appLayer);
    if (0 != _serviceToSend) {
        ExternalConfirmedServiceHandler::ActionToExecute todo;
        _serviceToSend->handleTimeout(&todo);
        if (ExternalConfirmedServiceHandler::ResendService == todo) {
            if (0 != appLayer) {
                appLayer->send(_destinedObject, _sourceAddress, _service, _serviceToSend);
                if (0 != action) *action = DeleteWrapper;
            } else {
                qDebug("%s : app layer not provided!", __PRETTY_FUNCTION__);
                if (0 != action) *action = DeleteAll;
                Q_ASSERT(false);
            }
        } else {
            if (0 != action) *action = DeleteAll;
        }
    } else {
        if (0 != action) *action = DeleteWrapper;
    }
}

void ConfirmedDiscoveryWrapper::deleteContents()
{
    delete _serviceToSend;
}

UnconfirmedDiscoveryWrapper::UnconfirmedDiscoveryWrapper(const ObjectIdStruct &destinedObject, const BacnetAddress &source, BacnetServiceData *data, quint8 serviceChoice):
    _destinedObject(destinedObject),
    _source(source),
    _data(data),
    _serviceChoice(serviceChoice)
{
}

quint32 UnconfirmedDiscoveryWrapper::handleTimeout(BacnetApplicationLayerHandler *appLayer, Action *action)
{
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(appLayer);
    Q_UNUSED(appLayer);
    if (0 != action)
        *action = DeleteAll;
}

void UnconfirmedDiscoveryWrapper::deleteContents()
{
    delete _data;
}


