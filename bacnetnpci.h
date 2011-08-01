#ifndef BACNETNPCI_H
#define BACNETNPCI_H

#include <QtCore>

#include "bitfields.h"
#include "bacnetaddress.h"
#include "bacnetcommon.h"

/**
6.2.2.1
DNET, SNET, and Vendor ID Encoding
The multi-octet fields, DNET, SNET, and Vendor ID, shall be conveyed with the most significant octet first. Allowable
network number values for DNET shall be from 1 to 65535 and for SNET from 1 to 65534.
  */

class BacnetNpci
{
public:

    enum SupportedProtocol {
        ProtocolVersion = 0x01
    };

    enum ParseReturnCodes {
        NpciBadProtocolVersion = -1,
        NpciInsaneControlField = -2
    };

    enum BacnetNetworkMessageType {
        WhoIsRouterToNetwork                                = 0x00,
        IAmRouterToNetwork                                  = 0x01,
        ICouldBeRouterToNetwork                             = 0x02,
        RejectMessageToNetwork                              = 0x03,
        RouterBusyToNetwork                                 = 0x04,
        RouterAvailableToNetwork                            = 0x05,
        InitializeRoutingTable                              = 0x06,
        InitializeRoutingTableAck                           = 0x07,
        EstablishConnectionToNetwork                        = 0x08,
        DisconnectConnectionToNetwork                       = 0x09,
        LastAshraeReserved                                  = 0x7F,
        FirstVendor                                         = 0x80,
        LastVendor                                          = 0xFF
    };

    BacnetNpci();

    /**
      Parses NPCI fields of the network message and stores information internally. Returns number of bytes used
      for consumint information (how many bytes were used to store it) or negative value otherwise.
      */
    qint8 setFromRaw(quint8 *inDataPrt);

    /**
      Fills the buffer starting at outDataPtr with information contained in the npci instance.
      \returns number of bytes used to store aforementioned information.
      */
    qint8 setToRaw(quint8 *outDataPtr);

    BacnetAddress &destAddress();
    void setDestAddress(BacnetAddress &addr);
    BacnetAddress &srcAddress();
    void setSrcAddress(BacnetAddress &addr);

    inline bool isSane() {return ((BitFields::Bit6 & _controlOctet) | (BitFields::Bit4 & _controlOctet)) == 0;}

    void setExpectingReply(bool expectReply);
    void setApduMessage();
    void setNetworkMessage(BacnetNetworkMessageType netMsgType);
    inline bool isNetworkLayerMessage() {return (BitFields::Bit7 & _controlOctet);}
    BacnetNetworkMessageType networkMessageType();

    inline BacnetCommon::NetworkPriority networkPriority() {
        return (BacnetCommon::NetworkPriority)((_controlOctet)&(BitFields::Bit0 | BitFields::Bit1));}
    inline void setNetworkPriority(BacnetCommon::NetworkPriority netPriority) {_controlOctet |= ( (quint8)netPriority & (BitFields::Bit0 | BitFields::Bit1) );}

private:
    inline bool isDestinationSpecified() {return (BitFields::Bit5 & _controlOctet);}
    inline bool isSourceSpecified() {return (BitFields::Bit3 & _controlOctet);}
    inline bool isConfirmed() {return (BitFields::Bit2 & _controlOctet);}
    inline bool isExpectingReply() {return (_controlOctet & BitFields::Bit2);}
    /**
      Decodes address bacnet address and network from NPCI
      \note affects netFieldPtr!
      */
    void decodeAddressHlpr(quint8 **netFieldPtr, BacnetAddress *outAddress);

    /**
      Encodes address bacnet address and network from bacAddress into buffer starting at netFieldPtr.
      \note affects netFieldPtr!
      */
    void encodeAddressHlpr(BacnetAddress &bacAddress, quint8 **netFieldPtr);

private:
    //! remembers control octet
    quint8 _controlOctet;
    //! stores information about srcAddr, from NPCI - if any
    BacnetAddress _srcAddr;
    //! stores information about destAddr, from NPCI - if any
    BacnetAddress _destAddr;
    //! hopCount info;
    quint8 _hopCount;
    //! message type
    BacnetNetworkMessageType _messageType;
    //! vendorId
    quint16 _vendorId;
};

#endif // BACNETNPCI_H
