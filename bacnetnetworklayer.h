#ifndef BACNETNETWORKLAYERHANDLER_H
#define BACNETNETWORKLAYERHANDLER_H

#include <QHash>

#include "bacnetaddress.h"
#include "bacnetnpci.h"


namespace Bacnet {class BacnetApplicationLayerHandler;}
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

    typedef quint16 TNetworkNum;
    typedef quint8 TPortId;

    /**
      Function should be invoked by lowe layer (transport layer) in order to parse the NPDU buffer.
      \param npdu - pointer to the start of the buffer containing NPDU data;
      \param length - length of the buffer, expressed as number of bytes from npdu pointer;
      \param dlSrcAddress - address of the source of this information;
      \param port - pointer to the BacnetTransportLayerHandler which this message comes from (would be useful for routing capability).
      */
    void readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress, BacnetTransportLayerHandler *port);

    void sendApdu(Buffer *apduBuffer, bool dataExpectingReply, const BacnetAddress *destAddr,
                  const BacnetAddress *srcAddr, Bacnet::NetworkPriority prio = Bacnet::PriorityNormal);

    /**
      Sets transport layer (or its proxy) that it communicates with, when wants to send data.
      The transport layer handler calls \sa readNpud method when it receives data which is meant
      to be passed to Application or Network layers.
      \param portId         - port Id, which is associatied with given transport handler
      \param transportHndlr - transport handler.
      */
    void addPorts(QHash<TPortId, BacnetTransportLayerHandler*> &transportHndlrs);

    void setApplicationLayer(Bacnet::BacnetApplicationLayerHandler *appHndlr);
    void setVirtualApplicationLayer(TNetworkNum virtualNetworkNum, Bacnet::BacnetApplicationLayerHandler *appHndlr);

    //! Adds the path to the network via specifier port. If the network was earlier reachable through other route, that route will be removed and only new remembered.
    void updateRoutingTableIndirectAccess(BacnetTransportLayerHandler *port, QVector<TNetworkNum> &indirectRouterNets, BacnetAddress &routerAddress);
    void updateRoutingTableIndirectAccess(TPortId portId, QVector<TNetworkNum> &indirectRouterNets, BacnetAddress &routerAddress);
    void rmNetworkIndirectAccess(BacnetTransportLayerHandler *port, quint16 net);

    //! Associates direct network num with portId. If there was such association, gets  deleted.
    void setPortDirectNetwork(TPortId portId, TNetworkNum networkNum);

    void broadcastAvailableNetworks();

private:
    /**
      Returns number of bytes used to process the message. If not successfully - negative.
      \param pointer to the byte, where network number starts (or nothing)
      \param length - length of the rest of network message
      \param port - pointer to the port transport layer - this would be very useful, if we were really a router, with different
      physical (or BVLL logical) ports.
      */
    qint32 processWhoIsRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetAddress &originAddress, BacnetNpci &npci, BacnetTransportLayerHandler *port);
    //! Helper function; returns all the router accessible networks except thos from port.
    QList<TNetworkNum> networksAccessibleExcludePort_helper(BacnetTransportLayerHandler *port);

    qint32 processRejectMessageToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port);
    qint32 processInitializeRoutingTable(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddr, BacnetTransportLayerHandler *port);
    qint32 processIAmRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddress, BacnetTransportLayerHandler *port);

    void sendRejectMessageToNetwork(RejectMessageToRouterReason rejReason, quint16 dnet, BacnetAddress &dlSenderAddress, BacnetTransportLayerHandler *port);

    void sendWhoIsRouterToNetwork(qint32 network, BacnetTransportLayerHandler *port);
    void prepareBufferWhoIsRouterToNetwork_hlpr(Buffer *buffer, qint32 network, BacnetNpci *npci, BacnetAddress *originAddr = 0);

    //! Creates and sends networks vector to the port. If originAddr is specified, this address will be inserted into NPCI SRC fields.
    void sendIAmRouterToNetwork_helper(QList<TNetworkNum> &networks, BacnetTransportLayerHandler *port);

    //! Used to send network messages, where dlDestination is already known (or 0, meainging b'cast) and port to which we want to send is known as well.
    void sendBuffer(Buffer *bufferToSend, Bacnet::NetworkPriority priority, BacnetTransportLayerHandler *port, const BacnetAddress *dlDestinationAddress = 0);

    /**
      This function returns the data link address that the message should be directed to in order to get to device with destAddr.
      Basically, if it's a remote message, it returns address of the router to dest network. Otherwise returns this address.
      */
//    const BacnetAddress *findAddrPortDestination(const BacnetAddress *destAddr);

private:
    //! Returns port Id. If not found, returns InvalidPortId.
    qint16 porrId(BacnetTransportLayerHandler *port);

    //! Returns network number directly connected to port.
    qint32 portDirectNetNum(BacnetTransportLayerHandler *port);


    /** Returns the port, that should be able to reach the given address (based on network parameters). If netwrork in address is provided, but the route is not found,
        returns NULL port and issues who-is-router-to-network request to all ports available.
        If the port is found and the network is not directly accessible (there is some intermediate router), it will also return pointer to the address of the next
        router on path. Otherwise it's NULL as well.
      */
    BacnetTransportLayerHandler *findDestinationForAddress(const BacnetAddress *destAddr, BacnetAddress *&dlAddress);

    /** Each routing entry should consist of direct network number, port number and a list of pair (networks accessible through this port, router address).
        \note we don't store direct network number in the entry RoutingEntry. It's already given as a hash key.
       */
    struct RoutingEntry {
        TPortId portId;
        BacnetTransportLayerHandler* port;
        QHash<TNetworkNum, BacnetAddress> indirectNetworkAccess;
    };
    static const quint8 AppLayerPortId = 0xff;
    static const quint8 InvalidPortId = -1;
    //! This is a Bacnet routing table. If the value of network is negative, it means
    QHash<TNetworkNum, RoutingEntry> _routingTable;
    QHash<TPortId, BacnetTransportLayerHandler*> _allPorts;
    /**
        Network number of the application application layer.
      */
    quint16 _virtualNetNum;
    Bacnet::BacnetApplicationLayerHandler *_virtualAppLayer;
//    /**
//      This hash stores information about routers that are connected to local network and are on the path to
//      the concrete network. The information is colleted wiht BacnetNpci::IAmRouterToNetwork network message.
//      Update mechanism is as follows. Either IAmRouterToNetwork comes from some reason - maybe some other device
//      requested.  But it can happen, that we have no such information and application entity wants to connect
//      to some network. We send app entity remote request with local broadcast (one of the local routers will grab it)
//      and just after that, we send WhoIsRouterToNetwork - then we will get desired information.
//      If the information gets stale - we will receive RejectMessageToNetwork with NotDirectlyConnected.
//      \todo \note \warning What if the router that is already set, changes its address? We would still have an old one, which doesn't
//      respond.
//      */

};

#endif // BACNETNETWORKLAYERHANDLER_H
