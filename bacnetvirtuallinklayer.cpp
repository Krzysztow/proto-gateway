#include "bacnetvirtuallinklayer.h"

#include <QtGlobal>
#include <QtEndian>
#include <QByteArray>

#include "bacnetbbmdhandler.h"
#include "bacnetudptransportlayer.h"
#include "bacnetnetworklayer.h"

BacnetBvllHandler::BacnetBvllHandler(BacnetNetworkLayerHandler *networkLayerHndlr,
                                     BacnetUdpTransportLayerHandler *transportLayerHndlr):
        _networkHndlr(networkLayerHndlr),
        _transportHndlr(transportLayerHndlr),
        _bbmdHndlr(0)
{
    Q_ASSERT(_networkHndlr != 0);
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
    if ( (*data) != BvllBacnetIpProtocol) {
        //don't parse anymore, not for us - we accept only BacnetIp microprotocol
        qDebug("Invalid microprotocol 0x%x", *data);
        return;
    }

    //read length from BVLC
    quint16 dataLength = qFromBigEndian(*(quint32*)&data[BvlcLengthField]);
    //assert length is really the one intended
    Q_ASSERT(dataLength == length);
    //we don't care about entire frame length anymore. Extract the data size.
    dataLength -= BvlcDataField;
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
                    setHeadersFields(respFrame, Read_Broadcast_Distribution_Table_Ack, respLength);
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
            //! \todo pass it to BVLL state machine, when we support FDT
            qDebug("BacnetBvllHandler: received Read_Foreign_Device_Table_Ack. NOT HANDLED!");
            break;
        }
    case (Delete_Foreign_Device_Table_Entry): {
            //! \todo pass it to BVLL state machine, when we support FDT
            sendShortResult(Delete_Foreign_Device_Table_Entry_NAK, srcAddr, srcPort);
            break;
        }
    case (Distribute_Broadcast_To_Network): {
            BacnetAddress srcBipAddr;
            BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &srcBipAddr);
            //get npdu data information (pointer to start & size)
            quint8 *npdu = data + BvlcConstHeaderSize;
            quint16 npduLength = dataLength - BvlcConstHeaderSize;

            //if we are BBMD, forward it to other BBMDS
            if (0 != _bbmdHndlr) {
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
            //some device sent us a directed message, forward it to the upper layer
            BacnetAddress srcBipAddr;
            BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &srcBipAddr);
            //prepare npdu infor
            quint8 *npdu = &data[BvlcConstHeaderSize];
            quint16 npduLength = dataLength - BvlcConstHeaderSize;
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
            quint16 npduLength = dataLength - BvlcConstHeaderSize;

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
        return;
    }
}

void BacnetBvllHandler::setHeadersFields(quint8 *data, BvlcFunction functionCode, quint16 length)
{
    //we assume that data points to the beginning of the frame
    //the data filled by the requester is requester specific and starts from data+BvlcConstHeaderSize

    quint8 *fieldsPtr = data;
    //set microprotocol
    *fieldsPtr = BvllBacnetIpProtocol;
    ++fieldsPtr;
    //set function code
    *fieldsPtr = (quint8)functionCode;
    ++fieldsPtr;
    //set the field size
    (*(quint16*)fieldsPtr) = qToBigEndian((quint16)length);
}

void BacnetBvllHandler::sendShortResult(BvlcResultCode result, QHostAddress destAddr, quint16 destPort)
{
    QByteArray respData;
    //BVLC-Result: Format - is constant length - 6 octets
    respData.reserve(BvlcResultSize);

    //fill headers
    quint8 *fieldsPtr = (quint8*)respData.data();
    setHeadersFields(fieldsPtr, BVLC_Result, BvlcResultSize);

    //set result value
    fieldsPtr += BvlcDataField;
    (*(quint16*)fieldsPtr) = qToBigEndian((quint16)result);

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
    setHeadersFields(fieldsPtr, Forwarded_NPDU, forwardedSize);
    fieldsPtr += BvlcConstHeaderSize;
    //create source/originator address
    BacnetAddress bIpAddress;
    BacnetBipAddressHelper::setMacAddress(srcAddr, srcPort, &bIpAddress);
    fieldsPtr += bIpAddress.macAddressToRaw(fieldsPtr);
    //copy npdu
    quint8 *npdu_hlpr = npduToForward;
    for (int i = 0; i<npduLength; i++) {
        *fieldsPtr = *npdu_hlpr;
        ++fieldsPtr;
        ++npdu_hlpr;
    }

    return forwardedSize;
}

void BacnetBvllHandler::sendNpdu(Buffer *buffToSend, BacnetCommon::NetworkPriority prio,
                                 const BacnetAddress *destAddress, const BacnetAddress *srcAddress)
{

}
