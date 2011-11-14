#include "bacnetnetworklayer.h"
#include "bacnettransportlayer.h"
#include "bacnetapplicationlayer.h"
#include "bacnetnpci.h"
#include "helpercoder.h"
#include "bacnetbuffermanager.h"


#define REAL_APP_LAYER_NUM -1

BacnetNetworkLayerHandler::BacnetNetworkLayerHandler():
        _transportHndlr(0)
{
}

void BacnetNetworkLayerHandler::sendRejectMessageToNetwork(RejectMessageToRouterReason rejReason, quint16 dnet, BacnetAddress &destAddr, BacnetTransportLayerHandler *port)
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
    sendBuffer(&buffer, Bacnet::PriorityNormal, &destAddr);
}

qint32 BacnetNetworkLayerHandler::processIAmRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddress, BacnetTransportLayerHandler *port)
{
    Q_UNUSED(port);
    /**followed by one or more 2-octet network numbers:
      Each network number is 2-bytes long. Thus we can expect length/2 networks to be passed.
      */
    Q_ASSERT((length%2) == 0);
    if (0 != (length%2)) {
        qDebug("BacnetNetworkLayerHandler: networks length is inappropriate!");
        return NotEnoughData;
    }

    quint8 netsNum = length%2;
    quint16 itNet;
    QVector<quint16> nets;
    nets.resize(netsNum);
    for (; netsNum > 0; --netsNum) {
        actualBytePtr += HelperCoder::uint16FromRaw(actualBytePtr, &itNet);
        nets.append(itNet);
    }

    addRouterToNetwork(nets, srcAddress);

    return length;
}

void BacnetNetworkLayerHandler::addRouterToNetwork(QVector<quint16> &nets, BacnetAddress &routerAddress)
{
    //override everything - this is the newest value possible, most probably the best one!
    for (int i=0; i<nets.count(); ++i) {
        _routers.insert(nets[i], routerAddress);
    }
}

void BacnetNetworkLayerHandler::rmRouterToNetwork(quint16 net)
{
    //remove all entreis (of course, should be only one)
    _routers.remove(net);
}

const BacnetAddress *BacnetNetworkLayerHandler::dlAddressForDest(const BacnetAddress *destAddr)
{
    static BacnetAddress localBroadcastAddr;
    localBroadcastAddr.setLocalBroadcast();

    if (0 == destAddr) {
        return &localBroadcastAddr;
    }

    Q_ASSERT(destAddr->isAddrInitialized());
    if (destAddr->hasNetworkNumber()) {
        if (destAddr->isGlobalBroadcast()) {
            return destAddr;
        }

        if (_routers.contains(destAddr->networkNumber())) {
            return &_routers[destAddr->networkNumber()];
        } else {
            //! \todo send who is router to network message!!!!!
#warning "Send who-is-router-to-network request!"
            return &localBroadcastAddr;
        }
    }

    return destAddr;
}

qint32 BacnetNetworkLayerHandler::processInitializeRoutingTable(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddr, BacnetTransportLayerHandler *port)
{
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
        /*6.4.7.: If an Initialize-Routing-Table message is sent
         with the Number of Ports equal to zero, the responding device
         shall return its complete routing table in an Initialize-Routing-
         Table-Ack message without updating its routing table.
         */
        //we assume that id for the port is it's number in a hash
        //this is not a good idea, since Qt docs say - With QHash, the items are arbitrarily ordered.
        quint8 *numberOfPorst = buffPtr;
        ++buffPtr;
        quint8 portId = 1;
        QHash<qint32, BacnetApplicationLayerHandler*>::Iterator portIt = _networks.begin();
        for (; portIt != _networks.end(); ++portIt) {
            if (portIt.key() >= 0) {//this is our virtual network
                if (buffLength - (buffPtr - buffer.bodyPtr()) < 3) return BufferToSmall;
                ++(*numberOfPorst);//increase number of sent ports
                //encode Connected DNET
                buffPtr += HelperCoder::uint32ToRaw(portId, buffPtr);
                ++portId;
                //encode port info length - we send no info, so length is 0
                *buffPtr = 0;
                ++buffPtr;
            }
        }
    }

    //being here, means we want to send data
    //set NPDU length; buffer start is not changed
    buffer.setBodyLength(buffPtr - buffer.bufferStart());

    //this is sent back to the source of the request
    sendBuffer(&buffer, Bacnet::PriorityNormal, &srcAddr);

    return (dataPtr - actualBytePtr);
}

qint32 BacnetNetworkLayerHandler::processRejectMessageToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port)
{
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
        rmRouterToNetwork(dnet);
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

qint32 BacnetNetworkLayerHandler::processWhoIsRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port)
{
    //the port would be used, if the real-physical routin was implemented
    Q_UNUSED(port);

    quint8 *dataPtr = actualBytePtr;
    QVector<quint16> networksToReturn;
    if (length > 0) {
        Q_ASSERT(2 == length);
        quint16 reqDnet;
        dataPtr += HelperCoder::uint16FromRaw(dataPtr, &reqDnet);
        if (_networks.contains(reqDnet)) {
            networksToReturn.append(reqDnet);
        }
    } else {
        /*
         Normally we should consult the routing table and extract networks not being on the port. Here we know
         that question could come only come from the only one port - thus we don't care, pass all the virtual
         networks.
         */
        QHash<qint32, BacnetApplicationLayerHandler*>::iterator netIt = _networks.begin();
        for (; netIt != _networks.end(); ++netIt) {
            if (netIt.key() >= 0) {//this is a virtual network, not an applicaton layer
                networksToReturn.append(netIt.key());
            }
        }
    }

    if (networksToReturn.count() == 0)
        return (dataPtr - actualBytePtr);//don't send anything - this was broadcast and we are not the ones who should answer

    //we have some networks to send
    //get the buffer to stuff
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::NetworkLayer);
    Q_ASSERT(buffer.isValid());
    //! \todo return
    if (!buffer.isValid())
        return BufferNotValid;

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
    Q_ASSERT( (buffLength - (buffPtr - buffer.bodyPtr())) >= (sizeof(quint16)*networksToReturn.count()));//assert we have enough place in the buffer
    QVector<quint16>::iterator netIt = networksToReturn.begin();
    for (; netIt != networksToReturn.end(); ++netIt) {
        buffPtr += HelperCoder::uin16ToRaw(*netIt, buffPtr);
    }

    //udpate buffer length. Start pointer hasn't changed.
    buffer.setBodyLength(buffPtr - buffer.bodyPtr());

    //lets send it
    /*who are we going to sent it to? - 6.6.3.2 says that we should send it with MAC broadcast,
    what means - send it with local broadcast - null pointer is enough*/
    sendBuffer(&buffer, Bacnet::PriorityNormal);

    return (dataPtr - actualBytePtr);
}

void BacnetNetworkLayerHandler::sendApdu(Buffer *apduBuffer, bool dataExpectingReply, const BacnetAddress *destAddr,
                                         const BacnetAddress *srcAddr, Bacnet::NetworkPriority prio)
{
#warning "Communication with Application layer not implemented, yet!"
    //remember to include the network address of the source
}
void BacnetNetworkLayerHandler::sendBuffer(Buffer *bufferToSend, Bacnet::NetworkPriority priority, const BacnetAddress *destination, const BacnetAddress *source)
{
    /** \note if this was a routing node, here we should consult a routing table and find which port (TransportLayerHndlr) should be called.
        So far we have only one port - no routing necessary!
      */

    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    *
    *
    *
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

    Buffer::printArray(bufferToSend->bodyPtr(), bufferToSend->bodyLength(), "Response from us (network):");

    const BacnetAddress *destAddress = dlAddressForDest(destination);
    _transportHndlr->sendNpdu(bufferToSend, priority, destAddress, source);
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
    ret = 0;
    if (npci.isNetworkLayerMessage()) {
        switch (npci.networkMessageType())
        {
        case (BacnetNpci::WhoIsRouterToNetwork):
            ret = processWhoIsRouterToNetwork(actualBytePtr, leftLength, port);
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
        BacnetApplicationLayerHandler *appHndlr = 0;
        if (npci.destAddress().isAddrInitialized()) {
            //was to be remote message - check network numbers
            appHndlr = _networks.value(npci.destAddress().networkNumber());
        } else {
            //was local message - pass it to the application layer
            appHndlr = _networks.value(REAL_APP_LAYER_NUM);
        }

        if (0 != appHndlr) {
            appHndlr->indication(actualBytePtr, leftLength,
                                 npci.srcAddress().isAddrInitialized() ? npci.srcAddress() : dlSrcAddress,
                                 npci.destAddress());
        }
        //else discard
    }
}

void BacnetNetworkLayerHandler::setTransportLayer(BacnetTransportLayerHandler *transportHndlr)
{
    //first make sure they are different. Otherwise we would be calling freed resources.
    if (_transportHndlr != transportHndlr) {
        delete _transportHndlr;
        _transportHndlr = transportHndlr;
    }
}

void BacnetNetworkLayerHandler::setApplicationLayer(BacnetApplicationLayerHandler *appHndlr)
{
    //they all are stored in a same fashion
    setVirtualApplicationLayer(REAL_APP_LAYER_NUM, appHndlr);
}

void BacnetNetworkLayerHandler::setVirtualApplicationLayer(quint16 virtualNetworkNum, BacnetApplicationLayerHandler *appHndlr)
{
    //we don't care if the application layer is null.
    if (_networks.contains(virtualNetworkNum)) {
        BacnetApplicationLayerHandler *oldApp = _networks.value(virtualNetworkNum);
        if (oldApp != appHndlr) {
            delete oldApp;
        }
    }
    //even if it existed and was the same, nothing bad happens.
    _networks.insert(virtualNetworkNum, appHndlr);
    if (0 != appHndlr) {
        appHndlr->setNetworkHandler(this);
    }
}
