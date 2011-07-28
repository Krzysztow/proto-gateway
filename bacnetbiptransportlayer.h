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

    void sendNpdu(quint8 *data, BacnetAddress &srcAddress, BacnetAddress &destAddress);

private:
    BacnetBvllHandler *_bvllHndlr;
    BacnetUdpTransportLayerHandler *_transportHndlr;
};

#endif // BACNETBIPTRANSPORTLAYER_H
