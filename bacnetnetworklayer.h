#ifndef BACNETNETWORKLAYERHANDLER_H
#define BACNETNETWORKLAYERHANDLER_H

#include <QHash>

#include "bacnetaddress.h"
#include "bacnetnpci.h"


class BacnetApplicationLayerHandler;
class BacnetTransportLayerHandler;
class Buffer;
class BacnetNetworkLayerHandler
{
public:
    BacnetNetworkLayerHandler();

    enum ErrorCodes {
        BufferNotValid = -1
    };

    /**
      Function should be invoked by lowe layer (transport layer) in order to parse the NPDU buffer.
      \param npdu - pointer to the start of the buffer containing NPDU data;
      \param length - length of the buffer, expressed as number of bytes from npdu pointer;
      \param dlSrcAddress - address of the source of this information;
      \param port - pointer to the BacnetTransportLayerHandler which this message comes from (would be useful for routing capability).
      */
    void readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress, BacnetTransportLayerHandler *port);

    void sendApdu(Buffer *apduBuffer, bool dataExpectingReply, BacnetAddress *destAddr,
                  BacnetAddress *srcAddr, BacnetCommon::NetworkPriority prio = BacnetCommon::PriorityNormal);

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
    /**
      Returns number of bytes used to process the message. If not successfully - negative.
      \param pointer to the byte, where network number starts (or nothing)
      \param length - length of the rest of network message
      \param port - pointer to the port transport layer - this would be very useful, if we were really a router, with different
      physical (or BVLL logical) ports.
      */
    qint32 processWhoIsRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port);

    void sendBuffer(Buffer *bufferToSend, BacnetCommon::NetworkPriority priority = BacnetCommon::PriorityNormal,
                    const BacnetAddress *destination = 0, const BacnetAddress *source = 0);

private:
    BacnetTransportLayerHandler *_transportHndlr;
    /**
      This one keeps track of the application layer and virtual networks supported by the device.
      Assumes, that the real application layer has -1 key, whereas virtual ones are positive and the
      key is same as it's network number.
      */
    QHash<qint32, BacnetApplicationLayerHandler*> _networks;

};

#endif // BACNETNETWORKLAYERHANDLER_H
