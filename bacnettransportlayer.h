#ifndef BACNETTRANSPORTLAYER_H
#define BACNETTRANSPORTLAYER_H

#include "bacnetnetworklayer.h"
#include "bacnetcommon.h"

/**
  This class provides interface for BACnet transport layer. The transport layer is supposed to interact only with
  # network layer should be able to invoke data transmission via \sa sendNpdu() method call;
  # this layer may pass data to an upper layer by invoking \sa readNpdu() method call;
  \sa BacnetUdpTransportLayer
  \sa BacnetNetworkLayer:
  */

class Buffer;
class BacnetTransportLayerHandler
{
public:
    /**
      This function gives \sa BacnetNetworkLayer ability to send data to the BACnet network.
      When no destAddress is specified-
      */
    virtual void sendNpdu(Buffer *buffToSend, BacnetCommon::NetworkPriority prio = BacnetCommon::PriorityNormal,
                          const BacnetAddress *destAddress = 0, const BacnetAddress *srcAddress = 0) = 0;
};

#endif // BACNETTRANSPORTLAYER_H
