#ifndef BACNETAPPLICATIONLAYERHANDLER_H
#define BACNETAPPLICATIONLAYERHANDLER_H

#include <QtCore>

#include "bacnetpci.h"

class BacnetAddress;
class BacnetNetworkLayerHandler;
class InternalObjectsHandler;
namespace Bacnet {
class ExternalObjectsHandler;
class BacnetTSM2;
}
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
    void indication(quint8 *data, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr);

    void processConfirmedRequest(quint8 *dataPtr, quint16 dataLength);

protected:
    BacnetNetworkLayerHandler *_networkHndlr;

    //! \todo should be changed when configuration is done with config files.
public:
    InternalObjectsHandler *_internalHandler;
    Bacnet::ExternalObjectsHandler *_externalHandler;
    Bacnet::BacnetTSM2 *_tsm;
};

#endif // BACNETAPPLICATIONLAYERHANDLER_H
