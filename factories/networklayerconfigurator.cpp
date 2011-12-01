#include "networklayerconfigurator.h"

#include "bacnetnetworklayer.h"

using namespace Bacnet;

BacnetNetworkLayerHandler *NetworkLayerConfigurator::createNetworkLayer(QDomElement &netCfgElement)
{
    Q_ASSERT(!netCfgElement.isNull());

    ::BacnetNetworkLayerHandler *netLayer = new ::BacnetNetworkLayerHandler();

    return netLayer;
}
