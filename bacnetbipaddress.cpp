#include "bacnetbipaddress.h"

#include <cstring>
#include <QtEndian>

#if (BIP_IP_LENGTH != 4)
#error "IPv6 not handled!"
#endif


quint8 BacnetBipAddressHelper::macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress)
{
    Q_ASSERT(0 != outAddress);
    outAddress->macAddressFromRaw(addrRawPtr, BipIpLength);
    //return number of bytes consumed
    return BipAddrLength;
}

//quint8 *BacnetBipAddress::address(quint8 &length)
//{
//    length = BIP_IP_LENGTH;
//    return _address;
//}

//quint8 *BacnetBipAddress::address()
//{
//    return _address;
//}


QHostAddress BacnetBipAddressHelper::ipAddress(BacnetAddress &inAddress)
{
    Q_ASSERT(inAddress.macAddrLength() == (BipAddrLength));
    if (inAddress.macAddrLength() == BipAddrLength) {
        return QHostAddress(qFromBigEndian(*(quint32*)inAddress.macPtr()));
    }
    else return QHostAddress();
}

quint16 BacnetBipAddressHelper::ipPort(BacnetAddress &inAddress)
{
    Q_ASSERT(inAddress.macAddrLength() == (BipAddrLength));
    if (inAddress.macAddrLength() == BipAddrLength) {
        return qFromBigEndian(*(quint16*)(inAddress.macPtr()[BipIpLength]));
    }
    else return 0;

}

quint16 BacnetBipAddressHelper::setMacAddress(QHostAddress inAddress, quint16 inPort, BacnetAddress *outAddress)
{
    Q_ASSERT(inAddress.protocol() == QAbstractSocket::IPv4Protocol);
    Q_ASSERT(0 != outAddress);

    //Bacnet Bip address is stored in a fashion that first 4 bytes are ip address in network byte order
    //and the other two are port in a network byte order

    quint8 tempMac[BipAddrLength];
    quint32 hostAddr = inAddress.toIPv4Address();
    *(quint32*)tempMac = qToBigEndian(hostAddr);
    quint8 *portField = &tempMac[BipIpLength];
    *(quint16*)portField = qToBigEndian(inPort);

    outAddress->macAddressFromRaw(tempMac, BipAddrLength);

    return BipAddrLength;
}
