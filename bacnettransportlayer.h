#ifndef BACNETTRANSPORTLAYER_H
#define BACNETTRANSPORTLAYER_H

#include "bacnetnetworklayer.h"

/**
  This class provides interface for BACnet transport layer. The transport layer is supposed to interact only with
  # network layer should be able to invoke data transmission via \sa sendNpdu() method call;
  # this layer may pass data to an upper layer by invoking \sa readNpdu() method call;
  \sa BacnetUdpTransportLayer
  \sa BacnetNetworkLayer:
  */

class BacnetTransportLayerHandler
{
public:
    /**
      This function gives \sa BacnetNetworkLayer ability to send data to the BACnet network.
      */
    virtual void sendNpdu(quint8 *data, BacnetAddress &srcAddress, BacnetAddress &destAddress) = 0;
};

#endif // BACNETTRANSPORTLAYER_H
