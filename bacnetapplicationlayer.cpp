#include "bacnetapplicationlayer.h"

BacnetApplicationLayerHandler::BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr):
        _networkHndlr(networkHndlr)
{
}

BacnetApplicationLayerHandler::~BacnetApplicationLayerHandler()
{
}

void BacnetApplicationLayerHandler::setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr)
{
    _networkHndlr = networkHndlr;
}
