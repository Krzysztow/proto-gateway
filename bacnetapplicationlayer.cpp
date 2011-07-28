#include "bacnetapplicationlayer.h"

BacnetApplicationLayerHandler::BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr):
        _networkHndlr(networkHndlr)
{

}

void BacnetApplicationLayerHandler::setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr)
{
    _networkHndlr = networkHndlr;
}
