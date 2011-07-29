#ifndef BACNETNPCI_H
#define BACNETNPCI_H

#include <QtCore>

#include "bitfields.h"
#include "bacnetaddress.h"

class BacnetNpci
{
public:
    enum NetworkPriority {
        PriorityNormal      = 0x00,
        PriorityUrgent      = 0x01,
        PriorityCritical    = 0x10,
        PriorityLifeSafety  = 0x11
    };

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

    inline bool isSane() {return ((BitFields::Bit6 & _controlOctet) | (BitFields::Bit4 & _controlOctet)) == 0;}
private:
    inline bool isNetworkLayerMessage() {return (BitFields::Bit7 & _controlOctet);}
    inline bool isDestinationSpecified() {return (BitFields::Bit5 & _controlOctet);}
    inline bool isSourceSpecified() {return (BitFields::Bit3 & _controlOctet);}
    inline bool isConfirmed() {return (BitFields::Bit2 & _controlOctet);}
    NetworkPriority networkPriority() {return (NetworkPriority)((_controlOctet)&(BitFields::Bit0 | BitFields::Bit1));}

    /**
      Decodes address bacnet address and network from NPCI
      \note affects inOutPtr!
      */
    void decodeAddressHlpr(quint8 **inOutPtr, BacnetAddress *outAddress);

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
};

#endif // BACNETNPCI_H
