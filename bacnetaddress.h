#ifndef BACNETADDRESS_H
#define BACNETADDRESS_H

#include <QtGlobal>

/**
  This class is intended to specify an abstract interface/helper of the Bacnet MAC layer address.
  */

class BacnetAddress
{
public:
    /**
     Sets data from the network byte order (big endian).
     \returns - number of bytes that were consumed to read the data.
     */
    virtual quint8 setFromRawData(quint8 *data, quint8 maxBytesToRead) = 0;

    /**
      Sets bytes starting from dest ptr to the value of the address.
      \note bytes are set in network byte order (big endian).
      \returns number of bytes that were used to store the address.
      */
    virtual quint8 setToRawData(quint8 *dest, quint8 maxBytesToSet) = 0;

    /**
      Returns number of bytes that are needed to store the address.
      */
    virtual quint8 length() = 0;

    /**
      Returns the pointer to the bytes representing address.
      \note THE DATA LAYOUT IS IN THE NETWORK ORDER (BIG ENDIAN)!!!
      */
    virtual quint8 *address(quint8 &length) = 0;
    virtual quint8 *address() = 0;

    /**
      Compares two addresses.
      */
    virtual bool isEqual(BacnetAddress &other) = 0;
};

#endif // BACNETADDRESS_H
