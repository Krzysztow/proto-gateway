#ifndef NETWORKLAYERCONFIGURATOR_H
#define NETWORKLAYERCONFIGURATOR_H

#include <QDomElement>
#include <QHash>

class BacnetNetworkLayerHandler;
class BacnetTransportLayerHandler;

namespace Bacnet {

class NetworkLayerConfigurator
{
public:
    static BacnetNetworkLayerHandler *createNetworkLayer(QHash<quint8, BacnetTransportLayerHandler*> &ports, QDomElement &netCfgElement);
};

}

#endif // NETWORKLAYERCONFIGURATOR_H
