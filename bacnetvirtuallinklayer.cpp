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
                QByteArray resultData; resultData.resize(dataSize + BvlcConstHeaderSize);
                quint8 *tblPtr = (quint8*)resultData.data() + BvlcDataField;
                bool ok = _bbmdHndlr->setBroatcastTableToRaw(tblPtr, dataSize);
                if (ok) {
                    sendBvlcFunction(Read_Broadcast_Distribution_Table_Ack, (quint8*)resultData.data(), resultData.size(),
                                     srcAddr, srcPort);
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
            //This type of frame can either be forwarded from BBMD (translated from broadcast on remote subnet)
            //or is sent to Foreign Device (broadcast on this subnet is translated into Forward-NPDU frame).
            //If we are BBMD, we have to take care of it, but be careful. If foreign BBMD used ip broadcast to pass this message here
            //we are forbidden to repeat it on our subnet - devices already consumed the data. Otherwise, it is a message passed directly
            //to us (2-hop broadcast). In this case we should broadcast it locally. How to check, how it was done? Remember all BBMDs
            //share the same BDT (not fully true - if there are two BBMDs on the subnet, their tables union is the same as union on other
            //subnets). So by looking on our FDT we can see how the other device got message to us - if the mask corresponding to our
            //BIP is all 1's, then 2nd case holds.
            BacnetBipAddress origDeviceAddress;
            quint8 addrLength = origDeviceAddress.length();
            quint8 *addrPtr = &data[Bvlc_Forwarded_AddressField];
            origDeviceAddress.setFromRawData(addrPtr, addrLength);

            //are we BBMD?
            if (0 != _bbmdHndlr) {
                _bbmdHndlr->processForwardedMessage(data, length);
            }

            //pass it to network layer
            quint8 *npduPtr = addrPtr + addrLength;
            //data passed to network layer is shorter than data carried by forwarded-NPDU by address length
            dataLength -= addrLength;
            _networkHndlr->readNpdu(npduPtr, dataLength, origDeviceAddress);

            break;
        }
    case (Register_Foreign_Device): {
            break;
        }
    case (Read_Foreign_Device_Table): {
            break;
        }
    case (Read_Foreign_Device_Table_Ack): {
            break;
        }
    case (Delete_Foreign_Device_Table_Entry): {
            break;
        }
    case (Distribute_Broadcast_To_Network): {
            break;
        }
    case (Original_Unicast_NPDU): {
            break;
        }
    case (Original_Broadcast_NPDU): {
            break;
        }
    default:
        return;
    }
}

void BacnetBvllHandler::sendShortResult(BvlcResultCode result, QHostAddress destAddr, quint16 destPort)
{
    QByteArray respData;
    //BVLC-Result: Format - is constant length - 6 octets
    respData.reserve(BvlcResultSize);

    quint8 *fieldsPtr = (quint8*)respData.data();
    //set microprotocol
    fieldsPtr += BvlcDataField;
    //set result
    (*(quint16*)fieldsPtr) = qToBigEndian((quint16)result);

    //send it
    sendBvlcFunction(BVLC_Result, (quint8*)respData.data(), BvlcResultSize, destAddr, destPort);
}

void BacnetBvllHandler::sendBvlcFunction(BvlcFunction functionCode, quint8 *data, quint16 length, QHostAddress destAddr, quint16 port)
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

    //send it
    Q_ASSERT(0 != _transportHndlr);
    _transportHndlr->send(data, length, destAddr, port);
}
