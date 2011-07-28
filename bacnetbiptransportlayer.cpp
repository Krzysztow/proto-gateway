#include "bacnetbiptransportlayer.h"

BacnetBipTransportLayer::BacnetBipTransportLayer(BacnetNetworkLayerHandler *networkHndlr):
        _transportHndlr(new BacnetUdpTransportLayerHandler())
{
    _bvllHndlr = new BacnetBvllHandler(networkHndlr, _transportHndlr);
    networkHndlr->setTransportLayer(this);
}

void BacnetBipTransportLayer::sendNpdu(quint8 *data, BacnetAddress &srcAddress, BacnetAddress &destAddress)
{
#warning "When BacnetBvllHandler has its send method, implemenet it here!"
//    _bvllHndlr->send
}
