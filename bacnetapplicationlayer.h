#ifndef BACNETAPPLICATIONLAYERHANDLER_H
#define BACNETAPPLICATIONLAYERHANDLER_H

class BacnetNetworkLayerHandler;
class BacnetApplicationLayerHandler
{
public:
    BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr);
    void setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr);

    BacnetNetworkLayerHandler *_networkHndlr;
};

#endif // BACNETAPPLICATIONLAYERHANDLER_H
