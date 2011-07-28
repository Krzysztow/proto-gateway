#include "bacnetbbmdhandler.h"

#include <QtEndian>

#include "bacnetudptransportlayer.h"

#if (BIP_IP_LENGTH != 4)
#error "IPv6 is not handled in this file"
#endif

BacnetBbmdHandler::BacnetBbmdHandler(BacnetBvllHandler *bvllHandler):
        _bvllHndlr(bvllHandler)
{
    Q_ASSERT(0 != _bvllHndlr);
}

bool BacnetBbmdHandler::setBroadcastTableFromRaw(quint8 *data, quint16 length)
{
    Q_ASSERT(0 != data);
    //first check if the lenght is of proper value
    //according to specifications (IPv6) is not handled here the data should be N*10octets
    quint8 bdtEntryLength = BacnetBipAddressHelper::BipAddrLength + BacnetBipAddressHelper::BipMaskLength;
    if (length%bdtEntryLength != 0) {
        return false;
        Q_ASSERT(false);
    }

    quint8 *fieldStart = data;
    quint8 numOfEntries = length/bdtEntryLength;
    Q_ASSERT(numOfEntries < 32);//probably we will never exceed that value
    //allocate enough memory for the table
    _bbmdTable.resize(numOfEntries);
    for (int i=0; i<numOfEntries; i++) {
        //read the address (IPv4 address and port)
        fieldStart += _bbmdTable[i].address.macAddressToRaw(fieldStart);
        //read the mask (length the same as IPvt4 port)
        //! \todo refactor these lines to functions to be independent of endianess
        *(quint16*)fieldStart = qFromBigEndian(_bbmdTable[i].mask);
        fieldStart += BacnetBipAddressHelper::BipMaskLength;
    }

    return true;
}

quint16 BacnetBbmdHandler::expectedBroatcastTableRawSize()
{
    quint8 bdtEntryLength = BacnetBipAddressHelper::BipAddrLength + BacnetBipAddressHelper::BipMaskLength;
    return _bbmdTable.size() * bdtEntryLength;
}

bool BacnetBbmdHandler::setBroatcastTableToRaw(quint8 *data, quint16 maxLength)
{
    Q_ASSERT(0 != data);
    //check how many entries we have and if they fit in maxLength
    quint8 bdtEntryLength = BacnetBipAddressHelper::BipAddrLength + BacnetBipAddressHelper::BipMaskLength;
    quint8 tableEntriesNum = _bbmdTable.size();
    if ( (bdtEntryLength*tableEntriesNum) >= maxLength ) {
        return false;
    }

    quint8 *fieldStart = data;
    for (int i=0; i<tableEntriesNum; i++) {
        //pass ip address & ip port
        fieldStart += _bbmdTable[i].address.macAddressToRaw(fieldStart);
        //pass ip broadcast mask
        *(quint16*)fieldStart = qToBigEndian(_bbmdTable[i].mask);
        fieldStart += BacnetBipAddressHelper::BipMaskLength;
    }

    return true;
}

void BacnetBbmdHandler::processForwardedMessage(quint8 *data, quint16 length, BacnetAddress &srcAddr)
{
    Q_ASSERT(0 != data);
    BacnetAddress myAddress;
    //get my ip and port from the transport layer and translate it to BacnetAddress
    BacnetBipAddressHelper::setMacAddress(_bvllHndlr->address(), _bvllHndlr->port(), &myAddress);
    //look the BDT for the entries with my BacnetIpAddress
    foreach (BbmdTableEntry entry, _bbmdTable) {
        //if entry is found, check how other devices forward data to me
        if (entry.address == myAddress) {
            /*is unicast used to broadcast?
            J.4.3.2 : If messages are to be distributed on the remote IP subnet by sending
            the message directly to the remote BBMD, the broadcast distribution mask shall be all 1's.
            */
            if ( entry.mask == (quint32)0xffffffff ) {
                //forward the message to our subnet, since no device has received it yet
                _bvllHndlr->send(data, length, QHostAddress::Broadcast, _bvllHndlr->port());
            }
            break;
        }
    }

    //we have written to our local network. if needed. But still we have to pass this frame to all FDs except source.
    #warning "Sending to entries in FDT (and entire FDT handling) not implemented"
    Q_UNUSED(srcAddr);
}

void BacnetBbmdHandler::sendToBbmds(quint8 *data, quint16 length, bool excludeMyself)
{
    BacnetAddress myAddress;
    //get my ip and port from the transport layer and translate it to BacnetBipAddress
    BacnetBipAddressHelper::setMacAddress(_bvllHndlr->address(), _bvllHndlr->port(), &myAddress);
    //look-up the BDT entries and send to all except from me
    QHostAddress toSendAddress;
    quint32 mask_helper, addr_helper;//two helpers for calculations
    foreach (BbmdTableEntry entry, _bbmdTable) {
        if (!excludeMyself || !(entry.address == myAddress)) {
            mask_helper = entry.mask;
            addr_helper = BacnetBipAddressHelper::ipAddress(entry.address).toIPv4Address();
            /*Calculate address we send message to:
              J.4.5: "The B/IP address to which the Forwarded-NPDU message is sent
              is formed by inverting the broadcast distribution mask in the BDT entry
              and logically ORing it with the BBMD address of the same entry."
             */
            mask_helper = ~mask_helper;//negate mask
            addr_helper |= mask_helper;//logical OR

            //resulting ip address
            toSendAddress.setAddress(addr_helper);
            //send to the calculated address
            _bvllHndlr->send(data, length, toSendAddress,BacnetBipAddressHelper::ipPort(entry.address));
        }
    }
}

void BacnetBbmdHandler::processBroadcastToNetwork(quint8 *data, quint16 length, BacnetAddress &srcAddr)
{
    Q_ASSERT(0 != data);

    //forward it locally
    //! \note remember that we won't get replication, since our UDP layer discards all messages received from itself
    _bvllHndlr->send(data, length, QHostAddress(QHostAddress::Broadcast), _bvllHndlr->port());

    sendToBbmds(data, length, true);

    //Message is now sent to BBMDs. But still we have to pass this frame to all FDs except source.
    #warning "Sending to entries in FDT (and entire FDT handling) not implemented"
    Q_UNUSED(srcAddr);
}

void BacnetBbmdHandler::processOriginalBroadcast(quint8 *data, quint16 length, BacnetAddress &srcAddr)
{
    Q_ASSERT(data);
    sendToBbmds(data, length, true);

    //Message is now sent to BBMDs. But still we have to pass this frame to all FDs except source.
    #warning "Sending to entries in FDT (and entire FDT handling) not implemented"
    Q_UNUSED(srcAddr);
}

//void BacnetBbmdHandler::processRegisterFD(quint8 *data, quint16 length, BacnetBipAddress &srcAddr)
//{
//    Q_ASSERT(length == 2);
//    quint16 ttl = qFromBigEndian(*(quint16*)data);
//    bool done = false;
//    for (QVector<FdTableEntry>::iterator it = _fdTable.begin(); it != _fdTable.end();  it++) {
//        if (it->address.isEqual(srcAddr)) {
//            it->timeToLiveLeft = ttl + 30;//additional time added of "fixed grace period" as stated in J.5.2.3
//            done = true;
//            break;
//        }
//    }

//    if (!done) {//was not found in already existing FDs
//        if (_fdTable.count() >= MAX_FD_NUM) {
//            qDebug("BacnetAddress::processRegisterFD() : FD not added!");
//            _bvllHndlr->sendShortResult(BacnetBvllHandler::Register_Foreign_Device_NAK, srcAddr.ipAddress(), srcAddr.ipPort());
//        }
//        else {
//            qDebug("BacnetAddress : registers new FD: %s : %d", qPrintable(srcAddr.ipAddress().toString()), srcAddr.ipPort());
//            FdTableEntry newFdEntry;
//            newFdEntry.address = srcAddr;
//            newFdEntry.timeToLiveLeft = ttl + 30;//same as above
//            _fdTable.append(newFdEntry);
//        }
//    }
//}
