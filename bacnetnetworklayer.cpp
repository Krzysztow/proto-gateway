#include "bacnetnetworklayer.h"
#include "bacnettransportlayer.h"
#include "bacnetapplicationlayer.h"
#include "bacnetnpci.h"
#include "helpercoder.h"
#include "bacnetbuffermanager.h"


#define REAL_APP_LAYER_NUM -1

BacnetNetworkLayerHandler::BacnetNetworkLayerHandler():
    _virtualAppLayer(0)
{
}

void BacnetNetworkLayerHandler::sendRejectMessageToNetwork(RejectMessageToRouterReason rejReason, quint16 dnet, BacnetAddress &dlSenderAddress, BacnetTransportLayerHandler *port)
{
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::NetworkLayer);
    Q_ASSERT(buffer.isValid());
    //! \todo return
    if (!buffer.isValid()) {
        qDebug("BacnetNetworkLayerHandler: can't send reject message - no valid buffer!");
        return;
    }
    quint16 buffLength = buffer.bodyLength();
    Q_ASSERT(buffLength > 0);
    quint8 *buffPtr = buffer.bodyPtr();
    BacnetNpci npci;
    npci.setNetworkMessage(BacnetNpci::RejectMessageToNetwork);
    npci.setExpectingReply(false);//this is a response
    npci.setNetworkPriority(Bacnet::PriorityNormal);
    //don't set npci addresses, since all is resent locally
    //fill buffer with NPCI
    buffPtr += npci.setToRaw(buffPtr);

    *buffPtr = (quint8)rejReason;
    ++buffPtr;
    buffPtr += HelperCoder::uin16ToRaw(dnet, buffPtr);

    buffer.setBodyLength(buffPtr - buffer.bodyPtr());

    sendBuffer(&buffer, Bacnet::PriorityNormal, port, &dlSenderAddress);
}

qint32 BacnetNetworkLayerHandler::processIAmRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddress, BacnetTransportLayerHandler *port)
{
    /**This is broadcast at each router startup onto each network. Each message contains each accessible netwokr, except those accessed from the one broadcast is made on.
      When received, it shall be broadcast locally

      Upon receipt of IARTN msg, I search my RotitingTable, for entries corresponding to  each network number and update them
      */

    Q_UNUSED(port);
    /**followed by one or more 2-octet network numbers:
      Each network number is 2-bytes long. Thus we can expect length/2 networks to be passed.
      */
    Q_ASSERT((length%2) == 0);
    if (0 != (length%2)) {
        qDebug("BacnetNetworkLayerHandler: networks length is inappropriate!");
        return NotEnoughData;
    }

    quint8 netsNum = length/2;
    quint16 itNet;
    QVector<TNetworkNum> nets;
    nets.resize(netsNum);
    for (int i = 0; i < netsNum; ++i) {
        actualBytePtr += HelperCoder::uint16FromRaw(actualBytePtr, &itNet);
        nets[i] = itNet;
    }

    //! \note optimization required? Think not, since used not often
    updateRoutingTableIndirectAccess(port, nets, srcAddress);

    return length;
}

qint16 BacnetNetworkLayerHandler::porrId(BacnetTransportLayerHandler *port)
{
    QHash<TPortId, BacnetTransportLayerHandler*>::Iterator it = _allPorts.begin();
    while (it != _allPorts.end()) {
        if (port == it.value())
            return it.key();
        ++it;
    }

    Q_ASSERT(false);//this shouldn't ever ahappen!
    return InvalidPortId;
}

void BacnetNetworkLayerHandler::updateRoutingTableIndirectAccess(TPortId portId, QVector<TNetworkNum> &indirectRouterNets, BacnetAddress &routerAddress)
{
    if (_allPorts.contains(portId))
        updateRoutingTableIndirectAccess(_allPorts[portId], indirectRouterNets, routerAddress);
}

void BacnetNetworkLayerHandler::setPortDirectNetwork(TPortId portId, TNetworkNum networkNum)
{
    BacnetTransportLayerHandler *port = _allPorts.value(portId);
    if (0 == port) {
        qDebug("%s : Couldn't set network (%d) for port (%d) : no such port!", __PRETTY_FUNCTION__, networkNum, portId);
        return;
    }

    //check if port was not used, buy some other network
    QHash<TNetworkNum, RoutingEntry>::Iterator rtIt = _routingTable.begin();
    while (rtIt != _routingTable.end()) {
        if (port == rtIt->port) {
            _routingTable.erase(rtIt);
            break;
        }
        ++rtIt;
    }

    RoutingEntry &rentry = _routingTable[networkNum];//if doesn't contain, operator[] creates element
    rentry.indirectNetworkAccess.clear();
    rentry.port = port;
    rentry.portId = portId;
}

void BacnetNetworkLayerHandler::updateRoutingTableIndirectAccess(BacnetTransportLayerHandler *port, QVector<TNetworkNum> &indirectRouterNets, BacnetAddress &routerAddress)
{
    //iterate over evey port to check there is no other entry to the network! Shouln't be. If so, delete!
    QHash<quint16, RoutingEntry>::Iterator reIt = _routingTable.begin();
    QHash<quint16, RoutingEntry>::Iterator reItEnd = _routingTable.end();
    for (; reIt != reItEnd; ++reIt) {
        if (reIt->port == port) {
            //this is where we want to insert given data
            for (int i = 0; i < indirectRouterNets.count(); ++i) {
                reIt->indirectNetworkAccess.insert(indirectRouterNets[i], routerAddress);
            }
        } else {
            //this is where we want to remove data, if duplicated
            for (int i = 0; i < indirectRouterNets.count(); ++i) {
                reIt->indirectNetworkAccess.remove(indirectRouterNets[i]);
            }
        }
    }
}

void BacnetNetworkLayerHandler::rmNetworkIndirectAccess(BacnetTransportLayerHandler *port, quint16 net)
{
    //remove all entreis (of course, should be only one)
    QHash<quint16, RoutingEntry>::Iterator reIt = _routingTable.begin();
    QHash<quint16, RoutingEntry>::Iterator reItEnd = _routingTable.end();
    for (; reIt!= reItEnd; ++reIt) {
        if (reIt->port == port) {
            reIt->indirectNetworkAccess.remove(net);
            return;
        }
    }
}

//const BacnetTransportLayerHandler *BacnetNetworkLayerHandler::findAddrPortDestination(const BacnetAddress &destAddr, const BacnetAddress *&routedDlDest)
//{
//    static BacnetAddress localBroadcastAddr;
//    localBroadcastAddr.setLocalBroadcast();

//    if (0 == destAddr) {
//        return &localBroadcastAddr;
//    }

//    Q_ASSERT(destAddr->isAddrInitialized());
//    if (destAddr->hasNetworkNumber()) {
//        if (destAddr->isGlobalBroadcast()) {
//            return destAddr;
//        }

//        if (_routers.contains(destAddr->networkNumber())) {
//            return &_routers[destAddr->networkNumber()];
//        } else {
//            //! \todo send who is router to network message!!!!!
//#warning "Send who-is-router-to-network request!"
//            return &localBroadcastAddr;
//        }
//    }

//    return destAddr;
//}

qint32 BacnetNetworkLayerHandler::processInitializeRoutingTable(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddr, BacnetTransportLayerHandler *port)
{
    /*$6.6.3.8
    An Initialize-Routing-Table message with Number of Ports set equal to zero shall cause the responding
    device to return its complete routing table in an Initialize-Routing-Table-Ack message without updating its routing table.

    When a router receives this message containing a routing table, indicated by a non-zero value in the Number of Ports field, it
    shall update its current port-to-network-number mappings for each network specified in the NPDU with the information
    contained in the NPDU and return an Initialize-Routing-Table-Ack message without any routing table data to the source.
    When a router receives this message in the form of a routing table query, indicated by a zero value in the Number of Ports
    field, it shall return an Initialize-Routing-Table-Ack message to the source containing a complete copy of its routing table as
    described in 6.6.3.9.
    */

    qDebug("Something 0");
    int c = _allPorts.count();
    qDebug("Something");
    int count = _routingTable.count();
    qDebug("Something 2 %d", count);

    Q_ASSERT(length >= 1);
    if (length < 1)
        return NotEnoughData;

    quint8 *dataPtr = actualBytePtr;
    quint8 portsNum = *dataPtr;
    dataPtr++;

    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::NetworkLayer);
    Q_ASSERT(buffer.isValid());
    //! \todo return
    if (!buffer.isValid())
        return BufferNotValid;
    quint16 buffLength = buffer.bodyLength();
    Q_ASSERT(buffLength > 0);
    quint8 *buffPtr = buffer.bodyPtr();
    BacnetNpci npci;
    npci.setNetworkMessage(BacnetNpci::InitializeRoutingTableAck);
    npci.setExpectingReply(false);//this is a response
    npci.setNetworkPriority(Bacnet::PriorityNormal);
    //don't set npci addresses, since all is resent locally
    //fill buffer with NPCI
    buffPtr += npci.setToRaw(buffPtr);

    if (0 != portsNum) {
        //we shouldn't take care of this information - parsing below is done just for the sake of it
        quint16 iterDnet;
        quint8 iterLength;
        quint16 bytesLeft = length - (dataPtr - actualBytePtr);
        for (; portsNum > 0; --portsNum) {
            //we need to read the dnet and port Infor length - 2 bytes + 1bytes necessary
            if (bytesLeft < 3) return NotEnoughData;
            dataPtr += HelperCoder::uint16FromRaw(dataPtr, &iterDnet);
            iterLength = *dataPtr;
            ++dataPtr; --bytesLeft;
            if (bytesLeft < iterLength) return NotEnoughData;
            //skip port information - we don't know what to do with it
            dataPtr += iterLength;
            bytesLeft -= iterLength;


        }
        //we are done with parsing, we will send simple ack
        *buffPtr = 0;//no ports are sent back, this is just ACK we got the request
        ++buffPtr;
    } else {
        //we assume that id for the port is it's number in a hash
        //this is not a good idea, since Qt docs say - With QHash, the items are arbitrarily ordered.
        quint8 *numberOfPorst = buffPtr;
        *numberOfPorst = _routingTable.count();
        ++buffPtr;

        //encode entry for virtual network (SNG)
        if (0 != _virtualAppLayer) {
            (*numberOfPorst) += 1;//1 is added, since we will add information about virtual port of application layer
            //add infor about app layer
            if (buffLength - (buffPtr - buffer.bodyPtr()) < 4) return BufferToSmall;
            //encode direct dnet
            buffPtr += HelperCoder::uin16ToRaw(_virtualNetNum, buffPtr);
            //encode port id
            *buffPtr = AppLayerPortId;
            ++buffPtr;
            //no port info, so length == 0
            *buffPtr = 0;
            ++buffPtr;
        }

        //encode routing table
        QHash<quint16, RoutingEntry>::Iterator portIt = _routingTable.begin();
        for (; portIt != _routingTable.end(); ++portIt) {
            if (buffLength - (buffPtr - buffer.bodyPtr()) < 4) return BufferToSmall;//we need 4 bytes to encode it
            //encode Connected DNET
            buffPtr += HelperCoder::uin16ToRaw(portIt.key(), buffPtr);
            //encode portId
            *buffPtr = portIt->portId;
            ++buffPtr;
            //encode port info length - we send no info, so length is 0
            *buffPtr = 0;
            ++buffPtr;
        }
    }
    //being here, means we want to send data
    //set NPDU length; buffer start is not changed
    buffer.setBodyLength(buffPtr - buffer.bufferStart());

    //this is sent back to the source of the request
    sendBuffer(&buffer, Bacnet::PriorityNormal, port, &srcAddr);

    return (dataPtr - actualBytePtr);
}

qint32 BacnetNetworkLayerHandler::processRejectMessageToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port)
{
    Q_UNUSED(port);
    quint8 *dataPtr = actualBytePtr;
    Q_ASSERT(length == 3);
    RejectMessageToRouterReason reasonCode = (RejectMessageToRouterReason)*dataPtr;
    const char *reason;
    ++dataPtr;
    quint16 dnet;
    dataPtr += HelperCoder::uint16FromRaw(actualBytePtr, &dnet);

    switch (reasonCode) {
    case (OtherError):
        reason = "Other error";
        break;
    case (NotDirectlyConnected):
        reason = "No direct connection";
        rmNetworkIndirectAccess(port, dnet);
        break;
    case (RouterBusy):
        reason = "Router is busy";
        break;
    case (UnknownNetworkMessage):
        reason = "Unknown message";
        break;
    case (TooLongMessage):
        reason = "Message sent is too long";
        break;
    default:
        reason = "Unrecognized reason";
    }

    qDebug("BacnetNetworkLayerHandler: got rejected network message from net#%d with reason: %s", dnet, reason);
    return (dataPtr - actualBytePtr);
}

qint32 BacnetNetworkLayerHandler::processWhoIsRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetNpci &rcvdNpci, BacnetTransportLayerHandler *port)
{
    /**
      If network router (even intermediate) cannot be found, return Reject 1.
      If the message is broadcast with the specific network number, one I-Am-Router-To-Network should be returned at most - from the router on
      a local network, that is on the path to the specified destination.
      If the network number is omitted - each router shall repolu with I-Am-Router-To-Network message, with all networks reachable through it.

       If found (not searching the table part with port which we got request from), send a I-Am-Router-To-Network with broadcast address to let others know.

       If network not found in the routing table, make an attempt to discover the next router on the path by generatign who-is-router-to-network and broadcast it on
       all other ports.

       1 # If WIRTN was from the originatin device - add SNET and SADR, before broadcasting. The responding device could i-COULD-be-router-to-network send unicast message.
       2 # From another router, containing SNET and SADR - just leave them there and resend.

       If we got no network number - construct I-Am... containgin list of all the networks, we are able to reach, excluding port->
*/


    quint8 *dataPtr = actualBytePtr;
    QVector<quint16> networksToReturn;
    if (length > 0) { // so the requester asks only for one, specific network
        Q_ASSERT(2 == length);
        quint16 reqDnet;
        dataPtr += HelperCoder::uint16FromRaw(dataPtr, &reqDnet);
        if (reqDnet == _virtualNetNum) { //is this our application layer?
            networksToReturn.append(reqDnet);
        } else { //so we are looking for some other layer
            QHash<quint16, RoutingEntry>::Iterator reIt = _routingTable.begin();
            QHash<quint16, RoutingEntry>::Iterator endIt = _routingTable.end();
            for (; reIt != endIt; ++reIt) {
                if ( (port != reIt->port) && //we don't want to investigate information from the same port
                     reIt->indirectNetworkAccess.contains(reqDnet) ) {
                        networksToReturn.append(reqDnet);
                        break;
                }
            }

            if (networksToReturn.isEmpty()) {
                //being here, means we have no access to the network - send  who-is-router-request on all ports.
                reIt = _routingTable.begin();
                for (; reIt != endIt; ++reIt) {
                    if (port != reIt->port) {
#warning "sendWhoIsRouterToNetworkRequest(network, sourceAddress ? npci.srcAddr())"
                    }
                }
            }
        }
    } else { //wants to read our reachable networks (excluding those that may be reached via this port)
        /*
         Normally we should consult the routing table and extract networks not being on the port. Here we know
         that question could come only come from the only one port - thus we don't care, pass all the virtual
         networks.
         */
        //first, we know we didn't
        //this is a virtual network, not an applicaton layer
        networksToReturn.append(_virtualNetNum);
    }

    if (!networksToReturn.isEmpty())
        sendIAmRouterToNetwork_helper(networksToReturn, port);

    return (dataPtr - actualBytePtr);
}

void BacnetNetworkLayerHandler::sendIAmRouterToNetwork_helper(QVector<quint16> &networks, BacnetTransportLayerHandler *port)
{
    //we have some networks to send
    //get the buffer to stuff
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::NetworkLayer);
    Q_ASSERT(buffer.isValid());
    //! \todo return
    if (!buffer.isValid())
        return;

    //we should be paying attention not to overrun buffer - write no more bytes than buffLength
    quint16 buffLength = buffer.bodyLength();
    Q_ASSERT(buffLength > 0);
    quint8 *buffPtr = buffer.bodyPtr();

    BacnetNpci npci;
    npci.setNetworkMessage(BacnetNpci::IAmRouterToNetwork);
    npci.setExpectingReply(false);//this is a response
    npci.setNetworkPriority(Bacnet::PriorityNormal);
    //don't set npci addresses, since all is resent locally

    //fill buffer with NPCI
    buffPtr += npci.setToRaw(buffPtr);

    //after npci is set, fill the buffer with the list of available networks
    Q_ASSERT( (buffLength - (buffPtr - buffer.bodyPtr())) >= (sizeof(quint16)*networks.count()));//assert we have enough place in the buffer
    QVector<quint16>::iterator netIt = networks.begin();
    for (; netIt != networks.end(); ++netIt) {
        buffPtr += HelperCoder::uin16ToRaw(*netIt, buffPtr);
    }

    //udpate buffer length. Start pointer hasn't changed.
    buffer.setBodyLength(buffPtr - buffer.bodyPtr());

    //lets send it
    /*who are we going to sent it to? - 6.6.3.2 says that we should send it with MAC broadcast,
    what means - send it with local broadcast - null pointer is enough*/
    sendBuffer(&buffer, Bacnet::PriorityNormal, port);
}

void BacnetNetworkLayerHandler::sendApdu(Buffer *apduBuffer, bool dataExpectingReply, const BacnetAddress *destAddr,
                                         const BacnetAddress *srcAddr, Bacnet::NetworkPriority prio)
{
    Q_UNUSED(prio);
    Q_UNUSED(dataExpectingReply);


#warning "Communication with Application layer not implemented, yet!"
    //remember to include the network address of the source
}

void BacnetNetworkLayerHandler::sendBuffer(Buffer *bufferToSend, Bacnet::NetworkPriority priority, BacnetTransportLayerHandler *port, const BacnetAddress *dlDestinationAddress)
{
    Buffer::printArray(bufferToSend->bodyPtr(), bufferToSend->bodyLength(), "Response from (network):");
    Q_CHECK_PTR(port);
    port->sendNpdu(bufferToSend, priority, dlDestinationAddress);
}

void BacnetNetworkLayerHandler::readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress, BacnetTransportLayerHandler *port)
{
    Buffer::printArray(npdu, length, "NPDU data: ");

    quint8 *actualBytePtr = npdu;
    quint16 leftLength = length;
    qint32 ret(0);

    //now it points at the NPCI field
    BacnetNpci npci;
    ret = npci.setFromRaw(actualBytePtr);
    Q_ASSERT(leftLength >= ret);
    Q_ASSERT(ret >= 0);
    Q_ASSERT(npci.isSane());
    if (ret < 0) {
        qDebug("BacnetNetworkLayerHandler: Insane frame, drop it!");
        return;
    }

    //we are pointning either at APDU or Network Message content - it depends what kind is this message of.
    actualBytePtr += ret;
    leftLength -= ret;

    HelperCoder::printArray(actualBytePtr, leftLength, "Data passed to network message:");

    ret = 0;
    if (npci.isNetworkLayerMessage()) {
        switch (npci.networkMessageType())
        {
        case (BacnetNpci::WhoIsRouterToNetwork):
            ret = processWhoIsRouterToNetwork(actualBytePtr, leftLength, npci, port);
            break;
        case (BacnetNpci::IAmRouterToNetwork):
            ret = processIAmRouterToNetwork(actualBytePtr, leftLength, dlSrcAddress, port);
            break;
        case (BacnetNpci::ICouldBeRouterToNetwork):
            /** \todo We do nothing about it so far. We have only one port - messages will be
                  sent to this one, no matter what the destination network number is.
                  Check also all the cases below, which are commented with: Same as above,
              */
            break;
        case (BacnetNpci::RejectMessageToNetwork):
            ret = processRejectMessageToNetwork(actualBytePtr, leftLength, port);
            break;
        case (BacnetNpci::RouterBusyToNetwork):
            //same as above
            break;
        case (BacnetNpci::RouterAvailableToNetwork):
            //same as above
            break;
        case (BacnetNpci::InitializeRoutingTable):
            ret = processInitializeRoutingTable(actualBytePtr, leftLength, dlSrcAddress, port);
            break;
        case (BacnetNpci::InitializeRoutingTableAck):
            //same as above
            break;
        case (BacnetNpci::EstablishConnectionToNetwork):
            //same as above
            break;
        case (BacnetNpci::DisconnectConnectionToNetwork):
            //same as above
            break;
        case (BacnetNpci::LastAshraeReserved):
            //same as above
            break;
        case (BacnetNpci::FirstVendor):
            //break;//fall through
        case (BacnetNpci::LastVendor):
            //break;//fall through
        default:
            sendRejectMessageToNetwork(UnknownNetworkMessage, npci.destAddress().networkNumber(), dlSrcAddress, port);//send unrecognized
        }

        if (ret < 0) {
            qDebug("Network message handled with error! Request: %d, error %d", npci.networkMessageType(), ret);
        }
        Q_ASSERT(ret >= 0);
    } else {
        /** This is either for application layer of:
            - the router device, when the message is locally sent (no DNET specified)
            - application layer of some of our virtual networks - if the DNET is specified and equal to one of registered networks in _networks.
            - for all application layers if this is a global broadcast
             Otherwise the message is to be dropped - it was not for us.
          */
        Bacnet::BacnetApplicationLayerHandler *appHndlr = 0;
        BacnetAddress destination(npci.destAddress());
        if (destination.isAddrInitialized()) {
            //was to be remote message - check network numbers
            if (destination.isGlobalBroadcast()) {
#warning "Ports are not included - send to applayer and to other ports"
                appHndlr = _virtualAppLayer;
                //! \todo distribute to other ports!
            } else if (destination.isRemoteBroadcast()) {
                if (destination.networkNumber() == _virtualNetNum)
                    appHndlr = _virtualAppLayer;
                else {
                    //! \todo Iterate over ports and check if there is no network directly connected to send to
                }
            } else if (destination.networkNumber() == _virtualNetNum)
                appHndlr = _virtualAppLayer;
        } else {
            //was local message - pass it to the application layer
//            appHndlr = _virtualNetNum.value(REAL_APP_LAYER_NUM);
            //handle local message!
        }

        if (0 != appHndlr) {
            appHndlr->indication(actualBytePtr, leftLength,
                                 npci.srcAddress().isAddrInitialized() ? npci.srcAddress() : dlSrcAddress,
                                 npci.destAddress());
        }
        //else discard
    }
}

void BacnetNetworkLayerHandler::addPorts(QHash<TPortId, BacnetTransportLayerHandler*> &transportHndlrs)
{
    QHash<TPortId, BacnetTransportLayerHandler*>::Iterator addedIt = transportHndlrs.begin();
    QList<BacnetTransportLayerHandler*> portsToDelete;
    for (; addedIt != transportHndlrs.end(); ++addedIt) {
        //find if port is not already there
        if (_allPorts.contains(addedIt.key()) && //if we already have such a portId...
                _allPorts[addedIt.key()] != addedIt.value()) { //...and corresponding port instance is not the same
            //mark port instance as to be deleted
            portsToDelete.append(_allPorts[addedIt.key()]);
            _allPorts[addedIt.key()] = addedIt.value();

            //clear all the routing info corresponding to port
            QHash<TNetworkNum, RoutingEntry>::Iterator rIt = _routingTable.begin();
            while (rIt != _routingTable.end()) {
                if (rIt->portId == addedIt.key())
                    rIt = _routingTable.erase(rIt);
                else
                    ++rIt;
            }
        } else { //the portId is new
            //it may be the case we are reusing a port and have it already in portsToDelete.
            //! \todo what if the port is reassigned?
            portsToDelete.removeOne(addedIt.value());
            _allPorts.insert(addedIt.key(), addedIt.value());
        }

        (*addedIt)->setNetworkLayer(this);
    }

    if (!portsToDelete.isEmpty()) {
        qDebug("%s : Some ports are going to be removed (# of ports %d)!", __PRETTY_FUNCTION__, portsToDelete.count());
        qDeleteAll(portsToDelete);
    }
}

void BacnetNetworkLayerHandler::setApplicationLayer(Bacnet::BacnetApplicationLayerHandler *appHndlr)
{
    //they all are stored in a same fashion
    setVirtualApplicationLayer(REAL_APP_LAYER_NUM, appHndlr);
}

void BacnetNetworkLayerHandler::setVirtualApplicationLayer(quint16 virtualNetworkNum, Bacnet::BacnetApplicationLayerHandler *appHndlr)
{
    //we don't care if the application layer is null.
    _virtualNetNum = virtualNetworkNum;
    if (_virtualAppLayer != appHndlr) {
        delete _virtualAppLayer;
        _virtualAppLayer = appHndlr;
        if (0 != appHndlr) {
            appHndlr->setNetworkHandler(this);
        }
    }
}

