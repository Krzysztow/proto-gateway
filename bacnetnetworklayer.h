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
        BufferNotValid = -1,
        BufferToSmall = -2,
        NotEnoughData = -3
    };

    enum RejectMessageToRouterReason {
        OtherError = 0,
        NotDirectlyConnected = 1,
        RouterBusy = 2,
        UnknownNetworkMessage = 3,
        TooLongMessage = 4
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
    qint32 processRejectMessageToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port);
    qint32 processInitializeRoutingTable(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddr, BacnetTransportLayerHandler *port);
    qint32 processIAmRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddress, BacnetTransportLayerHandler *port);

    void sendRejectMessageToNetwork(RejectMessageToRouterReason rejReason, quint16 dnet, BacnetAddress &destAddr, BacnetTransportLayerHandler *port);

    void sendBuffer(Buffer *bufferToSend, BacnetCommon::NetworkPriority priority = BacnetCommon::PriorityNormal,
                    const BacnetAddress *destination = 0, const BacnetAddress *source = 0);

    void addRouterToNetwork(QVector<quint16> &nets, BacnetAddress &routerAddress);
    void rmRouterToNetwork(quint16 net);
    /**
      This function returns the data link address that the message should be directed to in order to get to device with destAddr.
      Basically, if it's a remote message, it returns address of the router to dest network. Otherwise returns this address.
      */
    const BacnetAddress *dlAddressForDest(const BacnetAddress *destAddr);

private:
    BacnetTransportLayerHandler *_transportHndlr;
    /**
      This one keeps track of the application layer and virtual networks supported by the device.
      Assumes, that the real application layer has -1 key, whereas virtual ones are positive and the
      key is same as it's network number.
      */
    QHash<qint32, BacnetApplicationLayerHandler*> _networks;
    /**
      This hash stores information about routers that are connected to local network and are on the path to
      the concrete network. The information is colleted wiht BacnetNpci::IAmRouterToNetwork network message.
      Update mechanism is as follows. Either IAmRouterToNetwork comes from some reason - maybe some other device
      requested.  But it can happen, that we have no such information and application entity wants to connect
      to some network. We send app entity remote request with local broadcast (one of the local routers will grab it)
      and just after that, we send WhoIsRouterToNetwork - then we will get desired information.
      If the information gets stale - we will receive RejectMessageToNetwork with NotDirectlyConnected.
      \todo \note \warning What if the router that is already set, changes its address? We would still have an old one, which doesn't
      respond.
      */
    QHash<quint16 , BacnetAddress> _routers;

};

#endif // BACNETNETWORKLAYERHANDLER_H
