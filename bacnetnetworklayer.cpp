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

qint32 BacnetNetworkLayerHandler::processWhoIsRouterToNetwork(quint8 *actualBytePtr, quint16 length, BacnetTransportLayerHandler *port)
{
    //the port would be used, if the real-physical routin was implemented
    Q_UNUSED(port);

    quint16 usedBytes(0);
    QVector<quint16> networksToReturn;
    if (length > 0) {
        Q_ASSERT(2 == length);
        quint16 reqDnet;
        usedBytes += HelperCoder::uint16FromRaw(actualBytePtr, &reqDnet);
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
        return usedBytes;

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
    npci.setNetworkPriority(BacnetCommon::PriorityNormal);
    //don't set npci addresses, since all is resent locally

    //fill buffer with NPCI
    buffPtr += npci.setToRaw(buffPtr);

    //who are we going to sent it to? - 6.6.3.2 says that we should send it with MAC broadcast, what means - send it with local broadcast
    BacnetAddress destAddr;
    destAddr.setLocalBroadcast();

    //after npci is set, fill the buffer with the list of available networks
    Q_ASSERT( (buffLength - (buffPtr - buffer.bodyPtr())) >= (sizeof(quint16)*networksToReturn.count()));//assert we have enough place in the buffer
    QVector<quint16>::iterator netIt = networksToReturn.begin();
    for (; netIt != networksToReturn.end(); ++netIt) {
        buffPtr += HelperCoder::uin16ToRaw(*netIt, buffPtr);
    }

    //udpate buffer length. Start pointer hasn't changed.
    buffLength = buffPtr - actualBytePtr;
    buffer.setBodyLength(buffLength);

    //lets send it
    sendBuffer(&buffer, BacnetCommon::PriorityNormal, &destAddr);

    return (buffLength);
}

void BacnetNetworkLayerHandler::sendApdu(Buffer *apduBuffer, bool dataExpectingReply, BacnetAddress *destAddr,
              BacnetAddress *srcAddr, BacnetCommon::NetworkPriority prio)
{
#warning "Communication with Application layer not implemented, yet!"
}

void BacnetNetworkLayerHandler::sendBuffer(Buffer *bufferToSend, BacnetCommon::NetworkPriority priority, const BacnetAddress *destination, const BacnetAddress *source)
{
    /** \note if this was a routing node, here we should consult a routing table and find which port (TransportLayerHndlr) should be called.
        So far we have only one port - no routing necessary!
      */

    _transportHndlr->sendNpdu(bufferToSend, priority, destination, source);
}

void BacnetNetworkLayerHandler::readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress, BacnetTransportLayerHandler *port)
{

    //check if we can handle it - this class implements only de/en-coding of BACnet protocol version 1
    quint8 *actualBytePtr = npdu;
    qint32 ret;

    //now it points at the NPCI field
    actualBytePtr++;
    BacnetNpci npci;
    ret = npci.setFromRaw(actualBytePtr);
    Q_ASSERT(length >= ret);
    Q_ASSERT(ret < 0);
    Q_ASSERT(npci.isSane());
    if (ret < 0) {
        return;
    }

    //we are pointning either at APDU or Network Message content - it depends what kind is this message of.
    actualBytePtr += ret;

    if (npci.isNetworkLayerMessage()) {
        switch (npci.networkMessageType())
        {
        case (BacnetNpci::WhoIsRouterToNetwork):
                processWhoIsRouterToNetwork(actualBytePtr, (length - ret), port);
                break;
        case (BacnetNpci::IAmRouterToNetwork):
            break;
        case (BacnetNpci::ICouldBeRouterToNetwork):
            break;
        case (BacnetNpci::RejectMessageToNetwork):
            break;
        case (BacnetNpci::RouterBusyToNetwork):
            break;
        case (BacnetNpci::RouterAvailableToNetwork):
            break;
        case (BacnetNpci::InitializeRoutingTable):
            break;
        case (BacnetNpci::InitializeRoutingTableAck):
            break;
        case (BacnetNpci::EstablishConnectionToNetwork):
            break;
        case (BacnetNpci::DisconnectConnectionToNetwork):
            break;
        case (BacnetNpci::LastAshraeReserved):
            break;
        case (BacnetNpci::FirstVendor):
            //break;//fall through
        case (BacnetNpci::LastVendor):
            //break;//fall through
        default:
            ;//send unrecognized
        }
    } else {

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
