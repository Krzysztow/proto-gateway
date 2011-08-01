#include "bacnetbiptransportlayer.h"

#include "buffer.h"

BacnetBipTransportLayer::BacnetBipTransportLayer(BacnetNetworkLayerHandler *networkHndlr):
        _transportHndlr(new BacnetUdpTransportLayerHandler())
{
    _bvllHndlr = new BacnetBvllHandler(networkHndlr, _transportHndlr);
    networkHndlr->setTransportLayer(this);
    _bvllHndlr->setTransportProxy(this);
}

void BacnetBipTransportLayer::sendNpdu(Buffer *buffToSend, BacnetCommon::NetworkPriority prio,
                                       const BacnetAddress *destAddress, const BacnetAddress *srcAddress)
{
#warning "When BacnetBvllHandler has its send method, implemenet it here!"
//    _bvllHndlr->send
}

BacnetUdpTransportLayerHandler *BacnetBipTransportLayer::transportLayer()
{
    return _transportHndlr;
}

