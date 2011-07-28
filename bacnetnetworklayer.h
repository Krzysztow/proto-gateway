#ifndef BACNETNETWORKLAYERHANDLER_H
#define BACNETNETWORKLAYERHANDLER_H

#include <QHash>

#include "bacnetaddress.h"
#include "bitfields.h"

class BacnetApplicationLayerHandler;
class BacnetTransportLayerHandler;
class BacnetNetworkLayerHandler
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

    BacnetNetworkLayerHandler();

    void readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress);

    /**
      Sets transport layer (or its proxy) that it communicates with, when wants to send data.
      The transport layer handler calls \sa readNpud method when it receives data which is meant
      to be passed to Application or Network layers.
      \todo if we want to provide real routing - there should be few DLs possible to assign.
      */
    void setTransportLayer(BacnetTransportLayerHandler *transportHndlr);

    void setApplicationLayer(BacnetApplicationLayerHandler *appHndlr);
    void setVirtualApplicationLayer(quint16 virtualNetworkNum, BacnetApplicationLayerHandler *appHndlr);


private:
    BacnetTransportLayerHandler *_transportHndlr;
    /**
      This one keeps track of the application layer and virtual networks supported by the device.
      Assumes, that the real application layer has -1 key, whereas virtual ones are positive and the
      key is same as it's network number.
      */
    QHash<quint16, BacnetApplicationLayerHandler*> _networks;

};

class NpciFieldHelper {
public:
    static inline bool isNpciSane(quint8 *npciField) {return ((BitFields::Bit6 & *npciField) | (BitFields::Bit4 & *npciField)) == 0;}
    static inline bool isNetworkLayerMessage(quint8 *npciField) {return (BitFields::Bit7 & *npciField);}
    static inline bool isDestinationSpecified(quint8 *npciField) {return (BitFields::Bit5 & *npciField);}
    static inline bool isSourceSpecified(quint8 *npciField) {return (BitFields::Bit3 & *npciField);}
    static inline bool isConfirmed(quint8 *npciField) {return (BitFields::Bit2 & *npciField);}
    static BacnetNetworkLayerHandler::NetworkPriority networkPriority(quint8 *npciField)
        {return (BacnetNetworkLayerHandler::NetworkPriority)((*npciField)&(BitFields::Bit0 | BitFields::Bit1));}

};

#endif // BACNETNETWORKLAYERHANDLER_H
