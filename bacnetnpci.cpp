#include "bacnetnpci.h"

BacnetNpci::BacnetNpci():
        _controlOctet(0)
{
}

void BacnetNpci::decodeAddressHlpr(quint8 **dnetPtr, BacnetAddress *bacAddress)
{
    //network number is always available, then
    (*dnetPtr) += bacAddress->setNetworkNumFromRaw(*dnetPtr);
    //address is varying, dependant on MAC layer, so encode length first, then address itself
    //the length is 2 bytes long
    quint16 length = qFromBigEndian(*(quint16*)*dnetPtr);
    (*dnetPtr) += 2;
    //get address
    if (length == 0) {
        bacAddress->setRemoteBroadcast();
    } else {
        bacAddress->macAddressFromRaw(*dnetPtr, length);
        (*dnetPtr) += length;
    }
}

qint8 BacnetNpci::setFromRaw(quint8 *inDataPrt)
{
    quint8 *actualPtr = inDataPrt;
    if (*actualPtr != ProtocolVersion) {
#warning "What to do here? Just drop the frame, or send something back."
        return NpciBadProtocolVersion;
    }
    //remember control octet
    _controlOctet = *actualPtr;

    //check conformance with protocol
    if (!isSane()) {
        return NpciInsaneControlField;
    }

    //parse NPCI further
    actualPtr++;
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
        _hopCount = (*actualPtr) - 1;
        actualPtr++;
    }

    if (isNetworkLayerMessage()) {
        _messageType = (BacnetNetworkMessageType)*actualPtr;
        actualPtr++;
    }

    return (actualPtr - inDataPrt);
}
