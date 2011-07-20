#include "bacnetbipaddress.h"

#include <cstring>
#include <QtEndian>

#if (BIP_IP_LENGTH != 4)
#error "IPv6 not handled!"
#endif

BacnetBipAddress::BacnetBipAddress()
{
    memset(_address, 0, length());
}

quint8 BacnetBipAddress::setFromRawData(quint8 *addr, quint8 maxBytesToRead)
{
    quint16 lengthToCopy = length();
    Q_ASSERT(maxBytesToRead >= lengthToCopy);
    memcpy(_address, addr, lengthToCopy);
    //return number of bytes consumed
    return lengthToCopy;
}

quint8 BacnetBipAddress::setToRawData(quint8 *dest, quint8 maxBytesToSet)
{
    quint16 lengthToCopy = length();
    Q_ASSERT(maxBytesToSet >= lengthToCopy);
    //set address fields in network byte order
    memcpy(dest, _address, lengthToCopy);
    //return number of bytes stored
    return lengthToCopy;
}

quint8 BacnetBipAddress::length()
{
    return BIP_ADDR_LENGTH;
}

quint8 *BacnetBipAddress::address(quint8 &length)
{
    length = BIP_IP_LENGTH;
    return _address;
}

quint8 *BacnetBipAddress::address()
{
    return _address;
}

bool BacnetBipAddress::isEqual(BacnetAddress &other)
{
    //only IPv4 supported
    quint8 addrLength = length();
    //if other address has length different than BIP (so far only IPv4 supported) address length, surely they can't be the same
    if (other.length() == addrLength) {
        return (0 == memcmp(address(), other.address(), addrLength));
    }
    else
        return false;
}

QHostAddress BacnetBipAddress::ipAddress()
{
    return QHostAddress(qFromBigEndian(*(quint32*)_address));
}

quint16 BacnetBipAddress::ipPort()
{
    return qFromBigEndian(*(quint16*)(_address[BIP_IP_LENGTH]));
}

quint16 BacnetBipAddress::setAddress(QHostAddress address, quint16 port)
{
    Q_ASSERT(address.protocol() == QAbstractSocket::IPv4Protocol);

    quint32 hostAddr = address.toIPv4Address();
    *(quint32*)_address = qToBigEndian(hostAddr);
    quint8 *portField = _address + BIP_IP_LENGTH;
    *(quint16*)portField = qToBigEndian(port);

    return BIP_ADDR_LENGTH;
}

//===============================================


BacnetBipMask::BacnetBipMask()
{
    memset(_mask, 0, length());
}

quint8 BacnetBipMask::setFromRaw(quint8 *mask, quint8 maxBytesToRead)
{
    quint16 maskLength = length();
    Q_ASSERT(maskLength <= maxBytesToRead);
    memcpy(_mask, mask, maskLength);
    return maskLength;
}

quint8 BacnetBipMask::setToRaw(quint8 *dest, quint8 maxBytesToWrite)
{
    quint16 maskLength = length();
    Q_ASSERT(maskLength <= maxBytesToWrite);
    memcpy(dest, _mask, maskLength);
    return maskLength;

}

quint8 BacnetBipMask::length()
{
    return BIP_ADDR_LENGTH;
}

QHostAddress BacnetBipMask::mask()
{
    quint32 hostMask = qFromBigEndian(*(quint32*)_mask);
    return QHostAddress(hostMask);
}
