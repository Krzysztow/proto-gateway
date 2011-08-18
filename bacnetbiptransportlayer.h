#ifndef BACNETBIPTRANSPORTLAYER_H
#define BACNETBIPTRANSPORTLAYER_H

#include "bacnettransportlayer.h"
#include "bacnetnetworklayer.h"
#include "bacnetvirtuallinklayer.h"
#include "bacnetudptransportlayer.h"


class BacnetBipTransportLayer :
        public BacnetTransportLayerHandler
{
public:
    BacnetBipTransportLayer(BacnetNetworkLayerHandler *networkHndlr);

    /**
      Implementation of BacnetTransportLayerHandler interface function.
      This function is used to send
      */
    virtual void sendNpdu(Buffer *buffToSend, Bacnet::NetworkPriority prio = Bacnet::PriorityNormal,
                          const BacnetAddress *destAddress = 0, const BacnetAddress *srcAddress = 0);

    BacnetUdpTransportLayerHandler *transportLayer();

private:
    BacnetBvllHandler *_bvllHndlr;
    BacnetUdpTransportLayerHandler *_transportHndlr;
};

#endif // BACNETBIPTRANSPORTLAYER_H
