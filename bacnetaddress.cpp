#include "bacnetaddress.h"
#include <string.h>
#include <QtEndian>

BacnetAddress::BacnetAddress()
{
    resetMacAddress();
}

bool BacnetAddress::hasNetworkNumber()
{
    return (_networkNumber >= 0);
}

void BacnetAddress::setGlobalBroadcast()
{
    _networkNumber = GlobalBroadcastNet;
    _macAddrLength = 0;
}

bool BacnetAddress::isGlobalBroadcast()
{
    return (_networkNumber == GlobalBroadcastNet);
}

void BacnetAddress::setRemoteBroadcast()
{
    //network address must be set earlier or later
    _macAddrLength = 0;
}

bool BacnetAddress::isRemoteBroadcast()
{
    return ( (0 == _macAddress) && (_networkNumber >= 0) );
}

void BacnetAddress::setLocalBroadcast()
{
    _networkNumber = LocalBroadcastNet;
    _macAddrLength = 0;
}

bool BacnetAddress::isLocalBraodacst()
{
    return (_networkNumber == LocalBroadcastNet);
}

quint16 BacnetAddress::networkNumber()
{
    return _networkNumber;
}

void BacnetAddress::macAddressFromRaw(quint8 *data, quint8 length)
{
    _macAddrLength = length;
    memcpy(_macAddress, data, _macAddrLength);
}

quint8 BacnetAddress::macAddressToRaw(quint8 *data)
{
    memcpy(data, _macAddress, _macAddrLength);
    return _macAddrLength;
}

quint8 BacnetAddress::macAddrLength()
{
    return _macAddrLength;
}

quint8 *BacnetAddress::macPtr()
{
    return _macAddress;
}

quint8 BacnetAddress::networkNumToRaw(quint8 *data)
{
    if (hasNetworkNumber()) {
        *(quint16*)data = qToBigEndian((quint16)_networkNumber);
        return 2;
    }
    else
        return 0;
}

quint8 BacnetAddress::setNetworkNumFromRaw(quint8 *data)
{
    _networkNumber = qFromBigEndian(*(quint16*)data);
    return 2;
}

void BacnetAddress::resetMacAddress()
{
    _networkNumber = 0;
    _macAddrLength = -1;
    memset(_macAddress, 0, MaxMacLength);
}

bool BacnetAddress::isAddrInitialized()
{
    return (_macAddrLength > 0);
}

bool BacnetAddress::operator ==(BacnetAddress &other)
{
    quint8 addrLength = macAddrLength();
    //if other address has length different address length, surely they can't be the same
    if (other.macAddrLength() == addrLength) {
        return (0 == memcmp(macPtr(), other.macPtr(), addrLength));
    }
    else
        return false;
}
