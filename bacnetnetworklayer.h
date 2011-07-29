#ifndef BACNETNETWORKLAYERHANDLER_H
#define BACNETNETWORKLAYERHANDLER_H

#include <QHash>

#include "bacnetaddress.h"

class BacnetApplicationLayerHandler;
class BacnetTransportLayerHandler;
class BacnetNetworkLayerHandler
{
public:
    BacnetNetworkLayerHandler();

    void readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress);

    /**
      Sets transport layer (or its proxy) that it communicates with, when wants to send data.
      The transport layer handler calls \sa readNpud method when it receives data which is meant
      to be passed to Application or Network layers.
      \todo if we want to provide real routing - there should be few DLs possible to assign.
      */
    void setTransportLayer(BacnetTransportLayerHandler *transportHndlr);

    void setApplicationLayer(BacnetApplicationLayerHandler *appHndlr);
    void setVirtualApplicationLayer(quint16 virtualNetworkNum, BacnetApplicationLayerHandler *appHndlr);


private:
    BacnetTransportLayerHandler *_transportHndlr;
    /**
      This one keeps track of the application layer and virtual networks supported by the device.
      Assumes, that the real application layer has -1 key, whereas virtual ones are positive and the
      key is same as it's network number.
      */
    QHash<quint16, BacnetApplicationLayerHandler*> _networks;

};

#endif // BACNETNETWORKLAYERHANDLER_H
