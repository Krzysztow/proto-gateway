#include "bacnetbiptransportlayer.h"

#include "buffer.h"

BacnetBipTransportLayer::BacnetBipTransportLayer(BacnetNetworkLayerHandler *networkHndlr):
        _transportHndlr(new BacnetUdpTransportLayerHandler())
{
    _bvllHndlr = new BacnetBvllHandler(networkHndlr, _transportHndlr);
    _transportHndlr->setBvlc(_bvllHndlr);
    networkHndlr->setTransportLayer(this);
    _bvllHndlr->setTransportProxy(this);
}

void BacnetBipTransportLayer::sendNpdu(Buffer *buffToSend, BacnetCommon::NetworkPriority prio,
                                       const BacnetAddress *destAddress, const BacnetAddress *srcAddress)
{
#warning "When BacnetBvllHandler has its send method, implemenet it here!"
    _bvllHndlr->sendNpdu(buffToSend, prio, destAddress, srcAddress);
}

BacnetUdpTransportLayerHandler *BacnetBipTransportLayer::transportLayer()
{
    return _transportHndlr;
}

