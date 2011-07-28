#ifndef BACNETBIPADDRESS_H
#define BACNETBIPADDRESS_H

#include <QtGlobal>
#include <QHostAddress>

#include "bacnetaddress.h"
#define BIP_IP_LENGTH 4
/**
    This class provides functionality to manipulate BACnet/IP address.
    \note So far only IPv4 addresses are supported.
*/

class BacnetBipAddressHelper
{
public:
    enum {
        BipIpLength = BIP_IP_LENGTH,
        BipMaskLength = BipIpLength,
        BipPortLength = 2,
        BipAddrLength = BipIpLength + BipPortLength
    };

    /**
        Sets value of the address copying data from array starting at addr, considering it as the one containing Bip.
        \param addrRawPtr - pointer to the array field being a start byte of the address,
        \param outAddress - BacnetAddress to be set (has to be already allocated).
        \returns number of bytes that were used for the address
    */
    static quint8 macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress);

    /**
      Returns the ip address representing part of the B/IPs address from inAddress.
      \note it cannot be checked (apart from length) if the inAddress really represents BIP one. Call the function sensibly.
      */
    static QHostAddress ipAddress(BacnetAddress &inAddress);

    /**
      Returns ip port, being second part of the B/IP address.
      */
    static quint16 ipPort(BacnetAddress &inAddress);

    /**
      Converst QHostAddress (IPv4) and port into outAddress.
      \returns number of bytes used to store that data.
      */
    static quint16 setMacAddress(QHostAddress address, quint16 port, BacnetAddress *outAddress);
};


#endif // BACNETBIPADDRESS_H
