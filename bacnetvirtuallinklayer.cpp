#include "bacnetvirtuallinklayer.h"

#include <QtGlobal>
#include <QtEndian>
#include <QByteArray>

#include "bacnetbbmdhandler.h"
#include "bacnetudptransportlayer.h"
#include "bacnetnetworklayer.h"
#include "buffer.h"
#include "bacnetbuffermanager.h"
#include "helpercoder.h"

BacnetBvllHandler::BacnetBvllHandler(BacnetUdpTransportLayerHandler *transportLayerHndlr):
        _networkHndlr(0),
        _transportHndlr(transportLayerHndlr),
        _bbmdHndlr(0)
{
    _globBcastAddr.setGlobalBroadcast();
}

void BacnetBvllHandler::setNetworkLayer(BacnetNetworkLayerHandler *networkHndlr)
{
    _networkHndlr = networkHndlr;
}

void BacnetBvllHandler::setBbmdHndlr(BacnetBbmdHandler *bbmdHandler)
{
    _bbmdHndlr = bbmdHandler;
}

void BacnetBvllHandler::setTransportProxy(BacnetTransportLayerHandler *transportProxy)
{
    _transportProxyPtr = transportProxy;
}

void BacnetBvllHandler::consumeDatagram(quint8 *data, quint32 length, QHostAddress srcAddr, qint64 srcPort)
{
    //decode BVLL information
    //first assert there is enough data to encode this message
    Q_ASSERT(length >= BvlcConstHeaderSize);

    Buffer::printArray(data, length, "Buffer received:");

    if ( (*data) != BvllBacnetIpProtocol) {
        //don't parse anymore, not for us - we accept only BacnetIp microprotocol
        qDebug("Invalid microprotocol 0x%x", *data);
        return;
    }

    //read length from BVLC
    quint16 dataLength/* = qFromBigEndian(*(quint16*)&data[BvlcLengthField])*/;
    HelperCoder::uint16FromRaw(&data[BvlcLengthField], &dataLength);
    //assert length is really the one intended
    Q_ASSERT(dataLength == length);
    //we don't care about entire frame length anymore. Extract the data size.
    dataLength -= BvlcConstHeaderSize;
    //decide what to do with the frame by reading the BVLC function code
    switch (data[BvlcFuncField]) {
    case (BVLC_Result): {
            //! \todo pass it to Bvll state machine - it must be a response to BVLL layer query sent earlier
            break;
        }
    case (Write_Broadcast_Distribution_Table): {
            //if we are BBMD, take care of it
            if (_bbmdHndlr) {
                bool ok = _bbmdHndlr->setBroadcastTableFromRaw(&data[BvlcDataField], length);
                //send the result back to the originator
                sendShortResult((ok ? Successful_Completion : Write_Broadcast_Distribution_Table_NAK),
                                srcAddr, srcPort);
            }
            break;
        }
    case (Read_Broadcast_Distribution_Table): {
            //if we are BBMD take care of it
            //! \todo however I can't find in standard, what to do, when it is not a BBMD node - send NAK or discard message?
            if (_bbmdHndlr) {
                quint16 dataSize = _bbmdHndlr->expectedBroatcastTableRawSize();
                quint16 respLength = dataSize + BvlcConstHeaderSize;
                QByteArray resultData; resultData.resize(respLength);
                quint8 *respFrame = (quint8*)resultData.data();
                quint8 *tblPtr = &respFrame[BvlcDataField];
                bool ok = _bbmdHndlr->setBroatcastTableToRaw(tblPtr, dataSize);
                if (ok) {
                    setHeadersFields(respFrame, Read_Broadcast_Distribution_Table_Ack, dataSize);
                    send(respFrame, respLength, srcAddr, srcPort);
                }
                else {
                    sendShortResult(Read_Broadcast_Distribution_Table_NAK, srcAddr, srcPort);
                }
            }
            break;
        }
    case (Read_Broadcast_Distribution_Table_Ack): {
            //! \todo pass to BVLL state machine - this must be a response to Read_Broadcast_Distribution_Table it sent earlier
            qDebug("BacnetBvllHandler: received Read_Broadcast_Distribution_Table_Ack. NOT HANDLED!");
            break;
        }
    case (Forwarded_NPDU): {
            /*
            This type of frame can either be forwarded from BBMD (translated from broadcast on remote subnet)
            or is sent to Foreign Device (broadcast on this subnet is translated into Forward-NPDU frame).
            If we are BBMD, we have to take care of it, but be careful. If foreign BBMD used ip broadcast to pass this message here
            we are forbidden to repeat it on our subnet - devices already consumed the data. Otherwise, it is a message passed directly
            to us (2-hop broadcast). In this case we should broadcast it locally. How to check, how it was done? Remember all BBMDs
            share the same BDT (not fully true - if there are two BBMDs on the subnet, their tables union is the same as union on other
            subnets). So by looking on our FDT we can see how the other device got message to us - if the mask corresponding to our
            BIP is all 1's, then 2nd case holds.
            */
            BacnetAddress origDeviceAddress;
            quint8 *addrPtr = &data[Bvlc_Forwarded_AddressField];
            quint8 addrLength = BacnetBipAddressHelper::macAddressFromRaw(addrPtr, &origDeviceAddress);

            //do we support BBMD?
            if (0 != _bbmdHndlr) {
                //if so, check message and send if necessary
                _bbmdHndlr->processForwardedMessage(data, length, origDeviceAddress);
            }

            //pass it to network layer
            //data passed to network layer is shorter than data carried by forwarded-NPDU by address length
            quint8 *npduPtr = addrPtr + addrLength;
            dataLength -= addrLength;
            _networkHndlr->readNpdu(npduPtr, dataLength, origDeviceAddress, _transportProxyPtr);

            break;
        }
    case (Register_Foreign_Device): {
            //! \todo add support if necessary
            sendShortResult(Register_Foreign_Device_NAK, srcAddr, srcPort);
            break;
        }
    case (Read_Foreign_Device_Table): {
            //! \todo add support if necessary
            sendShortResult(Read_Foreign_Device_Table_NAK, srcAddr, srcPort);
            break;
        }
    case (Read_Foreign_Device_Table_Ack): {
            qDebug("BacnetBvllHandler: received Read_Foreign_Device_Table_Ack. Not interested!");
            break;
        }
    case (Delete_Foreign_Device_Table_Entry): {
        //! \todo pass it to BVLL state machine, when we support FDT
        if (0 != _bbmdHndlr) {
            qDebug("%s : Not handled, do it if FD is to be implemented.", __PRETTY_FUNCTION__);
            sendShortResult(Delete_Foreign_Device_Table_Entry_NAK, srcAddr, srcPort);
            break;
        }
    }
    case (Distribute_Broadcast_To_Network): {
        if (0 != _bbmdHndlr) {
            BacnetAddress srcBipAddr;
            BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &srcBipAddr);
            //get npdu data information (pointer to start & size)
            quint8 *npdu = data + BvlcConstHeaderSize;
            quint16 npduLength = dataLength;

            //if we are BBMD, forward it to other BBMDS
            //first create Forwarded-NPDU message
            QByteArray forwardedMsg;
            quint16 forwardedLength = createForwardedMsg(npdu, npduLength, forwardedMsg, srcAddr, srcPort);
            quint8 *forwardedPtr = (quint8 *)forwardedMsg.data();

            //this will send to all BBMDs except itself
            _bbmdHndlr->processBroadcastToNetwork(forwardedPtr, forwardedLength, srcBipAddr);
            /**
             \note this is important to pass message to the upper layer. This message was sent locally, but we will not
             receive it anymore, since our transport layer discards all the messages sent by oursleves.
             */
            //pass it to the netorowk layer, since we won't get the forwarded message (our transport layer blocks messages sent by ourself)
            _networkHndlr->readNpdu(npdu, npduLength, srcBipAddr, _transportProxyPtr);

        }
        break;
    }
    case (Original_Unicast_NPDU): {
            //some device sent us a directed message (it could be for us, or for some routed network - forward it upper)
            BacnetAddress srcBipAddr;
            BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &srcBipAddr);
            //prepare npdu infor
            quint8 *npdu = &data[BvlcConstHeaderSize];
            quint16 npduLength = dataLength;
            //pass it
            _networkHndlr->readNpdu(npdu, npduLength, srcBipAddr, _transportProxyPtr);
            break;
        }
    case (Original_Broadcast_NPDU): {
            //some device sent us a broadcast - forward it to BBMDs and FDs (if necessary) and pass to the upper layer
            BacnetAddress srcBipAddr;
            BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &srcBipAddr);
            //prepare npdu infor
            quint8 *npdu = &data[BvlcConstHeaderSize];
            quint16 npduLength = dataLength;

            //are we BBMD?
            if (0 != _bbmdHndlr) {
                QByteArray forwardedMsg;
                quint16 forwardedLength = createForwardedMsg(npdu, npduLength, forwardedMsg, srcAddr, srcPort);
                quint8 *forwardedPtr = (quint8 *)forwardedMsg.data();
                _bbmdHndlr->processOriginalBroadcast(forwardedPtr, forwardedLength, srcBipAddr);
            }

            //pass it to the upper layer
            _networkHndlr->readNpdu(npdu, npduLength, srcBipAddr, _transportProxyPtr);
            break;
        }
    default:
        Q_ASSERT_X(false, "BacnetBvllHandler", "Unrecognized BVLL message!");
        return;
    }
}

quint8 BacnetBvllHandler::setHeadersFields(quint8 *data, BvlcFunction functionCode, quint16 addLength)
{
    //we assume that data points to the beginning of the frame
    //the data that will be filled by the requester is requester specific and starts from data+BvlcConstHeaderSize
    //the size of the data appended is addLength
    //thus total size of the frame is addLength+BvlcConstHeaderSize

    //set microprotocol
    *data = BvllBacnetIpProtocol;
    ++data;
    //set function code
    *data= (quint8)functionCode;
    ++data;
    //set the field size
    HelperCoder::uin16ToRaw(addLength + BvlcConstHeaderSize, data);

    return BvlcConstHeaderSize;
}

void BacnetBvllHandler::sendShortResult(BvlcResultCode result, QHostAddress destAddr, quint16 destPort)
{
    QByteArray respData;
    //BVLC-Result: Format - is constant length - 6 octets
    respData.reserve(BvlcResultSize);

    //fill headers
    quint8 *fieldsPtr = (quint8*)respData.data();
    fieldsPtr += setHeadersFields(fieldsPtr, BVLC_Result, BvlcResultSize - BvlcConstHeaderSize);

    //set result value
    fieldsPtr += HelperCoder::uin16ToRaw(result, fieldsPtr);

    send((quint8*)respData.data(), BvlcResultSize, destAddr, destPort);
}

void BacnetBvllHandler::send(quint8 *data, quint16 length, QHostAddress destAddr, quint16 destPort)
{
    Q_ASSERT(0 != _transportHndlr);
    _transportHndlr->send(data, length, destAddr, destPort);
}

QHostAddress BacnetBvllHandler::address() {
    return _transportHndlr->address();
}

quint16 BacnetBvllHandler::port() {
    return _transportHndlr->port();
}

quint8 BacnetBvllHandler::createForwardedMsg(quint8 *npduToForward, quint16 npduLength, QByteArray &forwardedMsg, QHostAddress srcAddr, quint16 srcPort)
{
    quint16 forwardedSize = npduLength + BvlcConstHeaderSize + BacnetBipAddressHelper::BipAddrLength;
    forwardedMsg.reserve(forwardedSize);
    quint8 *forwardMsgPtr = (quint8*)forwardedMsg.data();
    quint8 *fieldsPtr = forwardMsgPtr;
    //fill in the message
    fieldsPtr += setHeadersFields(fieldsPtr, Forwarded_NPDU, forwardedSize - BvlcConstHeaderSize);
    //create source/originator address
    BacnetAddress bIpAddress;
    BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &bIpAddress);
    fieldsPtr += bIpAddress.macAddressToRaw(fieldsPtr);
    //copy npdu
    memcpy(fieldsPtr, npduToForward, npduLength);

    return forwardedSize;
}

void BacnetBvllHandler::sendNpdu(Buffer *buffToSend, Bacnet::NetworkPriority prio,
                                 const BacnetAddress *destAddress, const BacnetAddress *srcAddress)
{
    /* get safe start byte for this layer - it is calculated basing on the maximum header sizes for each layer
       We are sure that while creating correct header and inserting it's bytes we will not run onto bytes of NPDU
       which currently are at buffToSend->bodyStart().
     */
    quint8 *originalBufferStartPtr = buffToSend->bodyPtr();
    quint16 originalBodyLength = buffToSend->bodyLength();

    quint8 *startByte = buffToSend->bufferStart() + BacnetBufferManager::offsetForLayer(BacnetBufferManager::TransportLayer);
    quint8 *dataPtr = startByte;


    /** From network layer invoked send NPDU
        --------If is a broadcast:
        --------- send Original_Broadcast_NPDU with Subnet Broadcast address (to send to devices on my subnet)
        --------- to all entries in BDT (except myself) send Forwarded_NPDU - keep in mind hwo to create destination address for each subnet
        --------- to all entries in FDT send Forwarded_NPDU
        --------If is a localbroadcast || address DOES NOT have a network number:
        --------- send only to me and my FDs
        --------If is a remote broadcast || address HAS a network number:
        --------- send to all BBMDs


        NOT TRUE, because we cannot determine where (FDT or other BMBD) basing on network. Thus, when broadcast - send to Forwarded_NPDU to all entries in BDT (exccept mine) and all entries of FDT.
        Also send Original_Broadcast_NPDU to my subnet.

        When non-broadcast thing is sent from network, we just create  Unicast message and send to the source. (this will be a case of remote broadcast, the address will be unicast of the router).
      */

    /** If we receive message from our layer:
      - Write-Broadcast-Distribution-Table: Purpose
      - Read-Broadcast-Distribution-Table:
      - Read-Broadcast-Distribution-Table-Ack
      - Forwarded-NPDU: (BBMD sends it when got original-broadcast or distribute-broadcast)
        - forawrd on subnet (if BBMD and if not directed broadcast)
        - forward to every FD (if FD-BBMD except incoming)
        - forward to all FDTs (except incoming)
      - Register_Foreign_Device
      - Read_Foreign_Device_Table
      - Delete_Foreign_Device_Table_Entry
      - Distribute_Broadcast_To_Network (from FD):
        - Forwarded_NPDU to local subnet (if BBMD)                                                                            - local device gets Forwarded_NPDU as result of Bcast
        - Forwarded_NPDU to each BDT except myself (if BBMD)
        - Forwarded_NPDU to all FDTs (if FD-BBMD)
        - pass to network
      - Original-Unicast-NPDU ****************
      - Original-Broadcast-NPDU:
        - send Forwarded_NPDU to all devices in BDT except mine (if FD-BBMD)                                                     - bbmd local device gets Original_Broadcast_NPDU anyway, as a result of broadcast
        - send Forwarded_NPDU to all devices in FdTableEntry (if FD-BBMD)
        - pass to networ layer
      */

    /*althought BacnetAddress contains information about network - we don't use it here - this layer knows nothing about
      networking. It just makes check if the address is localBroadcast and if so send to QHostAddress::Broadcast address,
      unicast otherwise.
     */
    if (0 == destAddress)
        destAddress = &_globBcastAddr;
    if (destAddress->isLocalBraodacst() || destAddress->isGlobalBroadcast() || destAddress->isRemoteBroadcast()) {
        quint8 appendedSize(0);
        if (0 != _bbmdHndlr ) {
            //send as forwarded-NPDU to all FDs and BBMDs. Remember buffer information, so that at the end of this case, we restore them.
            //Forwarded-NPDU header without loosing information about NPDU position and size.
            //create original broadcast and send it
            dataPtr += setHeadersFields(dataPtr, Forwarded_NPDU,
                                        buffToSend->bodyLength() + BacnetBipAddressHelper::BipAddrLength);
            //set forwarded address
            BacnetBipAddressHelper::ipAddrToRaw(address(), port(), dataPtr);

            appendedSize = dataPtr - startByte;
            //we have to stitch the data together - there is probably a gap between npdu and this bvll header            
            Q_ASSERT(originalBufferStartPtr - dataPtr >= 0);//otherwise we overlap - this shouldn't happen
            quint8 *finalBodyPtr = startByte + (originalBufferStartPtr - dataPtr);//shift from startByte about gap width
            memmove(startByte, finalBodyPtr, appendedSize);//close the gap

            BacnetAddress tmpAddr;
            _bbmdHndlr->processOriginalBroadcast(finalBodyPtr, buffToSend->bodyLength() + appendedSize, tmpAddr);

            //restore buffer body (of APDU data) information
            buffToSend->setBodyPtr(originalBufferStartPtr);
            buffToSend->setBodyLength(originalBodyLength);
        }


        //create original-broadcast and send it with local braodcast address
        //this time I can use original buffer.
        dataPtr = startByte;
        qDebug("Network ptr 0x%x and apdu ptr 0x%x (No bbmd)", dataPtr, buffToSend->bodyPtr());
        dataPtr += setHeadersFields(dataPtr, Original_Broadcast_NPDU, buffToSend->bodyLength());
        appendedSize = dataPtr - startByte;
        Q_ASSERT(buffToSend->bodyPtr() - dataPtr >= 0);//otherwise we overlap - this shouldn't happen
        quint8 *finalBodyPtr = startByte + (buffToSend->bodyPtr() - dataPtr);//shift from startByte about gap width
        memmove(startByte, finalBodyPtr, appendedSize);//close the gap

        buffToSend->setBodyPtr(finalBodyPtr);
        buffToSend->setBodyLength(buffToSend->bodyLength() + appendedSize);
        QHostAddress broadcastAddr = QHostAddress::Broadcast;
        _transportHndlr->sendBuffer(buffToSend, broadcastAddr, port());

    } else {
        //create unicast and send it
        dataPtr += setHeadersFields(dataPtr, Original_Unicast_NPDU, buffToSend->bodyLength());
        quint8 appendedSize = dataPtr - startByte;
        //we have to stitch the data together - there is probably a gap between npdu and this bvll header
        Q_ASSERT(buffToSend->bodyPtr() - dataPtr >= 0);//otherwise we overlap - this shouldn't happen

        printf("Encoded size is %d (0x%x)\n", buffToSend->bodyLength(), buffToSend->bodyLength());
        HelperCoder::printArray(startByte, appendedSize, "BVLL stitch:");

        quint8 *finalBodyPtr = startByte + (buffToSend->bodyPtr() - dataPtr);//shift from startByte about gap width
        memmove(finalBodyPtr, startByte, appendedSize);//close the gap


        buffToSend->setBodyPtr(finalBodyPtr);
        buffToSend->setBodyLength(buffToSend->bodyLength() + appendedSize);

        Buffer::printArray(buffToSend->bodyPtr(), buffToSend->bodyLength(), "Bvll sends:");

        QHostAddress destHost = BacnetBipAddressHelper::ipAddress(*destAddress);
        quint64 destPort = BacnetBipAddressHelper::ipPort(*destAddress);

        _transportHndlr->sendBuffer(buffToSend, destHost, destPort);
    }

    //restore buffer data
    buffToSend->setBodyPtr(originalBufferStartPtr);
    buffToSend->setBodyLength(originalBodyLength);
}
