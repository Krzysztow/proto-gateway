#include "bacnetbbmdhandler.h"

#include "bacnetudptransportlayer.h"

#if (BIP_IP_LENGTH != 4)
#error "IPv6 is not handled in this file"
#endif

BacnetBbmdHandler::BacnetBbmdHandler(BacnetUdpTransportLayerHandler *transportHndlr):
        _transportHndlr(transportHndlr)
{
    Q_ASSERT(0 != _transportHndlr);
}

bool BacnetBbmdHandler::setBroadcastTableFromRaw(quint8 *data, quint16 length)
{
    Q_ASSERT(0 != data);
    //first check if the lenght is of proper value
    //according to specifications (IPv6) is not handled here the data should be N*10octets
    quint8 bdtEntryLength = BIP_ADDR_LENGTH + BIP_IP_LENGTH;
    if (length%bdtEntryLength != 0) {
        return false;
    }

    quint8 *fieldStart = data;
    quint8 numOfEntries = length/bdtEntryLength;
    Q_ASSERT(numOfEntries < 32);//probably we will never exceed that value
    //allocate enough memory for the table
    _bbmdTable.resize(numOfEntries);
    for (int i=0; i<numOfEntries; i++) {
        //read the address (IPv4 address and port)
        fieldStart += _bbmdTable[i].address.setFromRawData(fieldStart, BIP_ADDR_LENGTH);
        //read the mask (length the same as IPvt4 port)
        fieldStart += _bbmdTable[i].mask.setFromRaw(fieldStart, BIP_IP_LENGTH);
    }

    return true;
}

quint16 BacnetBbmdHandler::expectedBroatcastTableRawSize()
{
    quint8 bdtEntryLength = BIP_ADDR_LENGTH + BIP_IP_LENGTH;
    return _bbmdTable.size() * bdtEntryLength;
}

bool BacnetBbmdHandler::setBroatcastTableToRaw(quint8 *data, quint16 maxLength)
{
    Q_ASSERT(0 != data);
    //check how many entries we have and if they fit in maxLength
    quint8 bdtEntryLength = BIP_ADDR_LENGTH + BIP_IP_LENGTH;
    quint8 tableEntriesNum = _bbmdTable.size();
    if ( (bdtEntryLength*tableEntriesNum) >= maxLength ) {
        return false;
    }

    quint8 *fieldStart = data;
    for (int i=0; i<tableEntriesNum; i++) {
        //pass ip address & ip port
        fieldStart += _bbmdTable[i].address.setToRawData(fieldStart, BIP_ADDR_LENGTH);
        //pass ip broadcast mask
        fieldStart += _bbmdTable[i].mask.setToRaw(fieldStart, BIP_IP_LENGTH);
    }

    return true;
}

void BacnetBbmdHandler::processForwardedMessage(quint8 *data, quint16 length)
{
    Q_ASSERT(0 != data);
    BacnetBipAddress myAddress;
    //get my ip and port from the transport layer and translate it to BacnetBipAddress
    myAddress.setAddress(_transportHndlr->address(), _transportHndlr->port());
    //look the BDT for the entries with my BacnetIpAddress
    foreach (BbmdTableEntry entry, _bbmdTable) {
        //if entry is found, check how other devices forward data to me
        if (entry.address.isEqual(myAddress)) {
            /*is unicast used to broadcast?
            J.4.3.2 : If messages are to be distributed on the remote IP subnet by sending
            the message directly to the remote BBMD, the broadcast distribution mask shall be all 1's.
            */
            if ( entry.mask.mask() == QHostAddress((quint32)0xffffffff) ) {
                //forward the message to our subnet, since no device has read it yet
                _transportHndlr->send(data, length, QHostAddress::Broadcast, _transportHndlr->port());
            }
            break;
        }
    }

}
