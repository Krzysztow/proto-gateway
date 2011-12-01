#ifndef BACNET_TRANSPORTLAYERCONFIGURATOR_H
#define BACNET_TRANSPORTLAYERCONFIGURATOR_H

#include <QDomDocument>

class BacnetTransportLayerHandler;
class BacnetNetworkLayerHandler;
class BacnetBipTransportLayer;

namespace Bacnet {

class TransportLayerConfigurator
{
public:
    static ::BacnetTransportLayerHandler *createTransportLayer(BacnetNetworkLayerHandler *netLayer, QDomElement &transportLayCfg);

private:
    static BacnetBipTransportLayer *createBipTransportLayer(BacnetNetworkLayerHandler *netLayer, QDomElement &bipLayCfg);
};

} // namespace Bacnet

#endif // BACNET_TRANSPORTLAYERCONFIGURATOR_H
