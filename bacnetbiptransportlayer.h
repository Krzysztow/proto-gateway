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
    BacnetBipTransportLayer();

    virtual void setNetworkLayer(BacnetNetworkLayerHandler *networkHndlr);
    /**
      Implementation of BacnetTransportLayerHandler interface function.
      This function is used to send
      */
    virtual void sendNpdu(Buffer *buffToSend, Bacnet::NetworkPriority prio = Bacnet::PriorityNormal,
                          const BacnetAddress *destAddress = 0, const BacnetAddress *srcAddress = 0);

    BacnetUdpTransportLayerHandler *transportLayer();

private:
    BacnetUdpTransportLayerHandler *_transportHndlr;
    BacnetBvllHandler *_bvllHndlr;
};

#endif // BACNETBIPTRANSPORTLAYER_H
