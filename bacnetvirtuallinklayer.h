#ifndef BACNETBVLLHANDLER_H
#define BACNETBVLLHANDLER_H

#include <QByteArray>
#include <QHostAddress>

#include "bacnettransportlayer.h"
#include "bacnetcommon.h"

class BacnetNetworkLayerHandler;
class BacnetUdpTransportLayerHandler;
class BacnetBbmdHandler;
class Buffer;

/**
    \note The behaviour of Foreign Device is not implemented - most probably, it is easiest to exchange this Class,
    because only Forwarded-NPDU (for reception & transmission) as well as Distribute-Broadcast-To-Network (broadcast write)
    and Register-Foreign-Device with Result (registration is needed).

  */
class BacnetBvllHandler
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

    /**
      Used to send data from network layer.
      \note If destination address is 0, then local broadcast (using broadcast MAC address) is carried out.
      */
    virtual void sendNpdu(Buffer *buffToSend, BacnetCommon::NetworkPriority prio = BacnetCommon::PriorityNormal,
                          const BacnetAddress *destAddress = 0, const BacnetAddress *srcAddress = 0);


    //! Sets BBMD handler so that it may act as an BBMD.
    void setBbmdHndlr(BacnetBbmdHandler *bbmdHandler);
    void setTransportProxy(BacnetTransportLayerHandler *transportProxy);

private:
    //! Helper function to send data to underlying transport layer.
    void send(quint8 *data, quint16 length, QHostAddress destAddr, quint16 destPort);

    //! Get data from assigned transport layer about my ip address;
    QHostAddress address();

    //! Get my port from assigned transport layer
    quint16 port();

    //! Function creates BVLC-Result frame and pass it to the transport layer to send.
    void sendShortResult(BvlcResultCode result, QHostAddress destAddr, quint16 destPort);

    //! Helper function for filling microprotocol, function code and length fields.
    void setHeadersFields(quint8 *data, BvlcFunction functionCode, quint16 length);

    /**
      Helper function creating forwarded message from npdu. NPDU should be npduLength size.
      \param npduToForward - pointer to an array with npdu data to forward;
      \param forwardedMsg - array, where created message is stored;
      \param srcAddr - address of the originator of NPDU message;
      \param srcPort - port of the originator of NPDU message;
      \returns number of bytes that forwardedMsg takes.
      */
    quint8 createForwardedMsg(quint8 *npduToForward, quint16 npduLength, QByteArray &forwardedMsg, QHostAddress srcAddr, quint16 srcPort);

private:
    //! Holds the pointer to the network layer - should have.
    BacnetNetworkLayerHandler *_networkHndlr;
    //! Hodls pointer to the underlying transport layer - we know it's UDP.
    BacnetUdpTransportLayerHandler *_transportHndlr;
    //! Holds the pointer to the BBMD functionality, if any.
    BacnetBbmdHandler *_bbmdHndlr;

    //!just to pass to the network layer information about the TransportLayer we are in.
    BacnetTransportLayerHandler *_transportProxyPtr;

    friend class BacnetBbmdHandler;//encapsulation is broken! \todo think of it
};

#endif // BACNETBVLLHANDLER_H
