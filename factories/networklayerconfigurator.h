#ifndef NETWORKLAYERCONFIGURATOR_H
#define NETWORKLAYERCONFIGURATOR_H

#include <QDomElement>

class BacnetNetworkLayerHandler;

namespace Bacnet {

class NetworkLayerConfigurator
{
public:
    static ::BacnetNetworkLayerHandler *createNetworkLayer(QDomElement &netCfgElement);
};

}

#endif // NETWORKLAYERCONFIGURATOR_H
