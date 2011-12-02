#ifndef BACNET_TRANSPORTLAYERCONFIGURATOR_H
#define BACNET_TRANSPORTLAYERCONFIGURATOR_H

#include <QDomDocument>
#include <QHash>

class BacnetTransportLayerHandler;
class BacnetNetworkLayerHandler;
class BacnetBipTransportLayer;

namespace Bacnet {

class TransportLayerConfigurator
{
public:
    static QHash<quint8, BacnetTransportLayerHandler*> createTransportLayer(QDomElement &transportLayCfg);

private:
    static BacnetBipTransportLayer *createBipTransportLayer(QDomElement &bipLayCfg);
};

} // namespace Bacnet

#endif // BACNET_TRANSPORTLAYERCONFIGURATOR_H
