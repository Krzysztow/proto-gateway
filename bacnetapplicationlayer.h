#ifndef BACNETAPPLICATIONLAYERHANDLER_H
#define BACNETAPPLICATIONLAYERHANDLER_H

#include <QtCore>

class BacnetNetworkLayerHandler;
class BacnetAddress;
class BacnetApplicationLayerHandler
{
public:
    BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr);
    virtual ~BacnetApplicationLayerHandler();

    /**
      Sets the the network layer handler, that will be invoked whenever new message is to be sent.
      */
    void setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr);

    /**
      This is a method that should be invoked by Network Layer in order to pass data (starting at actualBytePtr*) of
      lenght length.
      \param - sourceAddr - is used by some requests/responses to localize the other device;
      \param - destAddr - not specified by BACnet but if application layer is supposed to act as a collection of
      devices we need to have information which device is being called.
      */
    virtual void indication(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr) = 0;

protected:
    BacnetNetworkLayerHandler *_networkHndlr;
};

#endif // BACNETAPPLICATIONLAYERHANDLER_H
