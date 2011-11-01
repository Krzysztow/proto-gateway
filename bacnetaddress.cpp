#include "bacnetaddress.h"
#include <string.h>
#include <QtEndian>

BacnetAddress::BacnetAddress()
{
    resetMacAddress();
}

bool BacnetAddress::hasNetworkNumber() const
{
    return (_networkNumber >= 0);
}

void BacnetAddress::setGlobalBroadcast()
{
    _networkNumber = GlobalBroadcastNet;
    _macAddrLength = 0;
}

bool BacnetAddress::isGlobalBroadcast() const
{
    return (_networkNumber == GlobalBroadcastNet);
}

void BacnetAddress::setRemoteBroadcast()
{
    //network address must be set earlier or later
    _macAddrLength = 0;
}

bool BacnetAddress::isRemoteBroadcast() const
{
    return ( (0 == _macAddress) && (_networkNumber >= 0) );
}

void BacnetAddress::setLocalBroadcast()
{
    _networkNumber = UninitizlizedNet;
    _macAddrLength = 0;
}

bool BacnetAddress::isLocalBraodacst() const
{
    return ( (0 == _macAddrLength) && (UninitizlizedNet == _networkNumber) );
}

quint16 BacnetAddress::networkNumber() const
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
    if (_macAddrLength >= 0) {
        memcpy(data, _macAddress, _macAddrLength);
        return _macAddrLength;
    }
    else {
        return 0;
    }
}

quint8 BacnetAddress::macAddrLength() const
{
    return _macAddrLength;
}

const quint8 *BacnetAddress::macPtr() const
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

void BacnetAddress::setNetworkNum(quint16 netNum)
{
    _networkNumber = netNum;
}

void BacnetAddress::resetMacAddress()
{
    _networkNumber = UninitizlizedNet;
    _macAddrLength = -1;
    memset(_macAddress, 0, MaxMacLength);
}

bool BacnetAddress::isAddrInitialized() const
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
