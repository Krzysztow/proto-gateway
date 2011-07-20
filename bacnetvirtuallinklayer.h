#ifndef BACNETBVLLHANDLER_H
#define BACNETBVLLHANDLER_H

#include <QByteArray>
#include <QHostAddress>

#include "bacnettransportlayer.h"

class BacnetNetworkLayerHandler;
class BacnetUdpTransportLayerHandler;
class BacnetBbmdHandler;

/**
    \note I wrap BacnetBvllHandler and it's BacnetUdpTransportLayer into BacnetTransportLayer - kind of proxy class.
    Thanks to that, other layers may use it as if it was normal BacnetTransportLayer.
  */
class BacnetBvllHandler:
        public BacnetTransportLayer
{
public:
    BacnetBvllHandler(BacnetNetworkLayerHandler *networkLayerHndlr,
                      BacnetUdpTransportLayerHandler *transportLayerHndlr);

    //! BACnet BVLC result codes
    enum BvlcResultCode{
        Successful_Completion,
        Write_Broadcast_Distribution_Table_NAK,
        Read_Broadcast_Distribution_Table_NAK,
        Register_Foreign_Device_NAK,
        Read_Foreign_Device_Table_NAK,
        Delete_Foreign_Device_Table_Entry_NAK,
        Distribute_Broadcast_To_Network_NAK
    };

    //! According to BACnet specification, there are 12 BVLC functions
    enum BvlcFunction {
        BVLC_Result                             = 0x00,
        Write_Broadcast_Distribution_Table      = 0x01,
        Read_Broadcast_Distribution_Table       = 0x02,
        Read_Broadcast_Distribution_Table_Ack   = 0x03,
        Forwarded_NPDU                          = 0x04,
        Register_Foreign_Device                 = 0x05,
        Read_Foreign_Device_Table               = 0x06,
        Read_Foreign_Device_Table_Ack           = 0x07,
        Delete_Foreign_Device_Table_Entry       = 0x08,
        Distribute_Broadcast_To_Network         = 0x09,
        Original_Unicast_NPDU                   = 0x0a,
        Original_Broadcast_NPDU                 = 0x0b
    };

    enum BvllMicroprotocol {
        BvllBacnetIpProtocol = 0x81
    };

    enum BvlcHeaderFieldPositionAndSize {
        BvlcProtoField = 0,
        BvlcFuncField = 1,
        BvlcLengthField = 2,
        BvlcDataField = 4,
        BvlcConstHeaderSize = BvlcDataField,

        BvlcResultSize = BvlcConstHeaderSize + 2,

        Bvlc_Forwarded_AddressField = BvlcDataField
    };

    //! Used when reading the data from underlying transport layer.
    void consumeDatagram(quint8 *data, quint32 datagramLength, QHostAddress srcAddr, qint64 srcPort);

    //! Sets BBMD handler so that it may act as an BBMD.
    void setBbmdHndlr(BacnetBbmdHandler *bbmdHandler);

private:
    //! Function creates BVLC-Result frame and pass it to the transport layer to send.
    void sendShortResult(BvlcResultCode result, QHostAddress destAddr, quint16 destPort);
    //! Helper function for sending BVLC data. Assumes that the specific data is already set and starts at data+BvlcConstHeaderSize
    void sendBvlcFunction(BvlcFunction functionCode, quint8 *data, quint16 length, QHostAddress destAddr, quint16 port);


private:
    //! Holds the pointer to the network layer - should have.
    BacnetNetworkLayerHandler *_networkHndlr;
    //! Hodls pointer to the underlying transport layer - we know it's UDP.
    BacnetUdpTransportLayerHandler *_transportHndlr;
    //! Holds the pointer to the BBMD functionality, if any.
    BacnetBbmdHandler *_bbmdHndlr;
    //! Holds the pointer to the FD functionality, if any.
};

#endif // BACNETBVLLHANDLER_H
