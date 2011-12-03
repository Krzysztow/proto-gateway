#include "bacnetnpci.h"
#include "helpercoder.h"

BacnetNpci::BacnetNpci():
        _controlOctet(0),
        _hopCount(255),
        _vendorId(0)
{
}

void BacnetNpci::decodeAddressHlpr(quint8 **netFieldPtr, BacnetAddress *bacAddress)
{
    quint8 *&ptr = *netFieldPtr;
    //network number is always available, then
    ptr += bacAddress->setNetworkNumFromRaw(ptr);
    //address is varying, dependant on MAC layer, so encode length first, then address itself
    //the length is 2 bytes long
    quint8 length = *ptr;
    ++ptr;
    //get address
    if (length == 0) {
        if (!bacAddress->isGlobalBroadcast())
            bacAddress->setIsRemoteBroadcast();
    } else {
        bacAddress->macAddressFromRaw(*netFieldPtr, length);
        (*netFieldPtr) += length;
    }
}

void BacnetNpci::encodeAddressHlpr(BacnetAddress &bacAddress, quint8 **netFieldPtr)
{
    quint8 *&ptr = *netFieldPtr;
    //network number is always available, then
    //! \todo This will not work if we were to rout between two irtual networks - no consultation with our routing table
    ptr += bacAddress.networkNumToRaw(ptr);
    //encode mac addresss length
    *ptr = (quint8)bacAddress.macAddrLength();
    ++ptr;
    //ptr += HelperCoder::uin16ToRaw(bacAddress.macAddrLength(), ptr);
    /*
    if it's not a remote and local broadcast (and it's not a local message (we are sure of that
    since network number is present)) we have to encode address as well - this is taken care of
    by BacnetAddress class.
    */
    ptr += bacAddress.macAddressToRaw(ptr);
}

qint8 BacnetNpci::setFromRaw(quint8 *inDataPrt)
{
    quint8 *actualPtr = inDataPrt;
    if (*actualPtr != ProtocolVersion) {
#warning "What to do here? Just drop the frame, or send something back."
        return NpciBadProtocolVersion;
    }
    ++actualPtr;

    //remember control octet
    _controlOctet = *actualPtr;

    //check conformance with protocol
    if (!isSane()) {
        return NpciInsaneControlField;
    }

    //parse NPCI further
    ++actualPtr;
    //do we have info about destination?
    if (isDestinationSpecified()) {
        //interpret information at inDataPrt as destination address. indataPtr gets updated by helper function.
        decodeAddressHlpr(&actualPtr, &_destAddr);
    }
    //do we have source information?
    if (isSourceSpecified()) {
        //interpret information at inDataPrt as source address. indataPtr gets updated by helper function.
        decodeAddressHlpr(&actualPtr, &_srcAddr);
    }
    //if there was destination, we have hop count as well - decrease the hop count
    if (isDestinationSpecified()) {
        _hopCount = *actualPtr;
        ++actualPtr;
    }

    //take care of message type and vendor id
    if (isNetworkLayerMessage()) {
        _messageType = (BacnetNetworkMessageType)*actualPtr;
        ++actualPtr;
        if (_messageType > LastAshraeReserved) {
            actualPtr += HelperCoder::uint16FromRaw(actualPtr, &_vendorId);
        }
    }

    return (actualPtr - inDataPrt);
}

qint8 BacnetNpci::setToRaw(quint8 *outDataPtr)
{
    quint8 *actPtr(outDataPtr);
    //set protocol version
    *actPtr = ProtocolVersion;
    ++actPtr;

    //we have to check few things before control octet is ready
    if (_destAddr.hasNetworkNumber()) {
        _controlOctet |= BitFields::Bit5;
    }
    if (_srcAddr.hasNetworkNumber()) {
        //it is a remote message or broadcast - SNET, SLEN will be present
        _controlOctet |= BitFields::Bit3;
    }
    //expecting reply should be already set
    //priority should be already set

    //set control octet
    *actPtr = _controlOctet;
    ++actPtr;
    //if we have destination set, we need to insert it
    if (isDestinationSpecified()) {
        encodeAddressHlpr(_destAddr, &actPtr);
    }
    //if we have source set
    if (isSourceSpecified()) {
        encodeAddressHlpr(_srcAddr, &actPtr);
    }

    //hop count
    if (isDestinationSpecified()) {
        //if it's a routed message - we already decreased
        *actPtr = (_hopCount - 1);
        ++actPtr;
    }

    //vendor id
    if (isNetworkLayerMessage() ) {
        *actPtr = (quint8)_messageType;
        ++actPtr;
        if ((networkMessageType() > LastAshraeReserved))
            *actPtr += HelperCoder::uin16ToRaw(_vendorId, actPtr);
    }

    return (actPtr - outDataPtr);
}

BacnetNpci::BacnetNetworkMessageType BacnetNpci::networkMessageType()
{
    if (isNetworkLayerMessage())
        return _messageType;
    else
        return LastVendor;
    enum NetworkPriority {
        PriorityNormal      = 0x00,
        PriorityUrgent      = 0x01,
        PriorityCritical    = 0x10,
        PriorityLifeSafety  = 0x11
    };
}

void BacnetNpci::setExpectingReply(bool expectReply)
{
    if (expectReply)
        _controlOctet |= BitFields::Bit2;
    else
        _controlOctet &= (~BitFields::Bit2);
}

void BacnetNpci::setApduMessage()
{
    _controlOctet &= (~BitFields::Bit7);
    //maybe shouldn't set this one?
    _messageType = LastVendor;
}

void BacnetNpci::setNetworkMessage(BacnetNetworkMessageType netMsgType)
{
    //set network message flag in the control octet
    _controlOctet |= BitFields::Bit7;
    //remember message type
    _messageType = netMsgType;
}

BacnetAddress &BacnetNpci::destAddress()
{
    //! \todo = maybe I should check if control bit is set?
    return _destAddr;
}

void BacnetNpci::setDestAddress(const BacnetAddress &addr)
{
    _controlOctet |= BitFields::Bit5;
    _destAddr = addr;
}

BacnetAddress &BacnetNpci::srcAddress()
{
    //! \todo = maybe I should check if control bit is set?
    return _srcAddr;
}

void BacnetNpci::setSrcAddress(const BacnetAddress &addr)
{
    _controlOctet |= BitFields::Bit3;
    _srcAddr = addr;
}
