#ifndef BACNETBIPADDRESS_H
#define BACNETBIPADDRESS_H

#include <QtGlobal>
#include <QHostAddress>

#include "bacnetaddress.h"

/**
    This class provides functionality to manipulate BACnet/IP address.
    \note So far only IPv4 addresses are supported.
*/

#define BIP_IP_LENGTH 4
#define BIP_PORT_LENGTH 2
#define BIP_ADDR_LENGTH BIP_IP_LENGTH+BIP_PORT_LENGTH

class BacnetBipAddress:
        public BacnetAddress
{
public:
    BacnetBipAddress();

//Implemented pure virtual functions from BacnetAddress

    /**
        Sets value of the address copying data from array of addr. This function takes care of the byte order
        of the input addr. Assumes th
        \param addr - pointer to quint8, that is a first byte of the address (first ip address is laid, then the port associated)
        \param isNetworkOrder - indicates if the layout of bytes is in the network order or host
        \returns number of bytes that were used for the address
        \sa BacnetAddress::setFromRaw()
    */
    quint8 setFromRawData(quint8 *data, quint8 maxBytesToRead);
    /**

      \sa BacnetAddress::setToRaw()
      */
    quint8 setToRawData(quint8 *dest, quint8 maxBytesToSet);

    /**
      All BACnet MAC addresses have length less than 255 octets - enough to store.
      \sa Bacnet::length()
      */
    quint8 length();

    /**
      Compares two addresses.
      */
    bool isEqual(BacnetAddress &other);

    /**
      Returns byte representation of the address (in network order).
      */

    quint8 *address(quint8 &length);
    quint8 *address();

//Concrete class functions

    /**
      Returns the ip address representing part of the B/IPs
      */
    QHostAddress ipAddress();
    /**
      Returns ip port, being second part of the B/IP address.
      */
    quint16 ipPort();

    quint16 setAddress(QHostAddress address, quint16 port);

private:
    quint8 _address[BIP_ADDR_LENGTH];
};

class BacnetBipMask {

public:
    BacnetBipMask();

    /**
     These functions works the same way, as BacnetBipAddress functions do
     \sa BacnetBipAddress
     */
    quint8 setFromRaw(quint8 *addr, quint8 maxBytesToRead);
    quint8 setToRaw(quint8 *dest, quint8 maxBytesToWrite);

    quint8 length();
    QHostAddress mask();

private:
    quint8 _mask[BIP_IP_LENGTH];
};

#endif // BACNETBIPADDRESS_H
