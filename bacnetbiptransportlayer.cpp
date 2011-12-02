#include "bacnetbiptransportlayer.h"

#include "buffer.h"

BacnetBipTransportLayer::BacnetBipTransportLayer():
    _transportHndlr(new BacnetUdpTransportLayerHandler()),
    _bvllHndlr(new BacnetBvllHandler(_transportHndlr))
{
    _transportHndlr->setBvlc(_bvllHndlr);
    _bvllHndlr->setTransportProxy(this);
}

void BacnetBipTransportLayer::setNetworkLayer(BacnetNetworkLayerHandler *networkHndlr)
{
    _bvllHndlr->setNetworkLayer(networkHndlr);
}

void BacnetBipTransportLayer::sendNpdu(Buffer *buffToSend, Bacnet::NetworkPriority prio,
                                       const BacnetAddress *destAddress, const BacnetAddress *srcAddress)
{
    _bvllHndlr->sendNpdu(buffToSend, prio, destAddress, srcAddress);
}

BacnetUdpTransportLayerHandler *BacnetBipTransportLayer::transportLayer()
{
    return _transportHndlr;
}

