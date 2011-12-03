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

void BacnetAddress::setIsRemoteBroadcast()
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
    return (_macAddrLength >= 0) ? _macAddrLength : 0;
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
    return ( (_macAddrLength > 0) || isGlobalBroadcast() );
}

bool BacnetAddress::operator ==(const BacnetAddress &other) const
{
    quint8 addrLength = macAddrLength();
    //if other address has length different address length, surely they can't be the same
    if (other.macAddrLength() == addrLength) {
        return (0 == memcmp(macPtr(), other.macPtr(), addrLength));
    }
    else
        return false;
}

#include <QStringList>

static const char *MacSeparator = ":";

bool BacnetAddress::macAddressFromString(QString &addressStr)
{
    QStringList macParts = addressStr.split(MacSeparator);
    quint8 macSize = macParts.count();
    if ( (macSize > MaxMacLength) ||
         (macSize <= 0) )
        return false;
    bool ok;
    memset(_macAddress, 0, MaxMacLength);
    for (int i = 0; i < macSize; ++i) {
        _macAddress[i] = macParts.at(i).toUInt(&ok, 16);
        if (!ok)
            break;
    }

    if (!ok) {
        memset(_macAddress, 0, MaxMacLength);
        _macAddrLength = -1;
    } else {
        _macAddrLength = macSize;
    }

    return ok;
}

bool BacnetAddress::networkNumFromString(QString &netStr)
{
    bool ok;
    _networkNumber = netStr.toUInt(&ok, 0);
    if (!ok)
        _networkNumber = UninitizlizedNet;

    return ok;
}

QString BacnetAddress::macAddressToString()
{
    QString macStr;
    for (int i = 0; i < _macAddrLength; ++i) {
        macStr += QString::number(_macAddress[i], 16);
        if (_macAddrLength - 1 != i)
            macStr += ":";
    }

    return macStr;
}

QString BacnetAddress::netToString()
{
    if (UninitizlizedNet == _networkNumber)
        return QString();
    return QString::number(_networkNumber);
}
