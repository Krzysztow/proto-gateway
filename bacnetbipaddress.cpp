#include "bacnetbipaddress.h"

#include <cstring>
#include <QString>
#include "helpercoder.h"

#if (BIP_IP_LENGTH != 4)
#error "IPv6 not handled!"
#endif
static const char *BacnetBipAddressSeparator = ":";

bool BacnetBipAddressHelper::macAddressFromString(QString &addrStr, QHostAddress *host, quint64 *port)
{
    //expected format is xxx.xxx.xxx.xxx:<port-num>, all numbers in decimal
    QStringList addrList = addrStr.split(BacnetBipAddressSeparator);
    if (addrList.count() == 0)
        return false;

    host->setAddress(addrList.first());
    bool ok(false);
    if (addrList.count() == 2) {
        *port = addrList.at(1).toUInt(&ok);
    }
    if (!ok)
        *port = 0xBAC0;//assume default port
    return true;
}

quint8 BacnetBipAddressHelper::macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress)
{
    Q_ASSERT(0 != outAddress);
    outAddress->macAddressFromRaw(addrRawPtr, BipAddrLength);
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


QHostAddress BacnetBipAddressHelper::ipAddress(const BacnetAddress &inAddress)
{
    Q_ASSERT(inAddress.macAddrLength() == (BipAddrLength));
    if (inAddress.macAddrLength() == BipAddrLength) {
        quint32 ipNum;
        HelperCoder::uint32FromRaw(inAddress.macPtr(), &ipNum);
//        qDebug()<<"--- address:"<<ipNum<<QHostAddress(ipNum).toString();
//        HelperCoder::printArray(inAddress.macPtr(), BipIpLength, "--- address waas:");
        return QHostAddress(ipNum);
    }
    else return QHostAddress();
}

quint16 BacnetBipAddressHelper::ipPort(const BacnetAddress &inAddress)
{
    Q_ASSERT(inAddress.macAddrLength() == (BipAddrLength));

    const quint8 *ptr = &(inAddress.macPtr()[BipIpLength]);

    if (inAddress.macAddrLength() == BipAddrLength) {
        quint16 portHostOrder;
        HelperCoder::uint16FromRaw(ptr, &portHostOrder);
        return portHostOrder;
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
//    quint32 hostAddr = inAddress.toIPv4Address();
//    *(quint32*)tempMac = qToBigEndian(hostAddr);
//    quint8 *portField = &tempMac[BipIpLength];
//    *(quint16*)portField = qToBigEndian(inPort);

    ipAddrToRaw(inAddress, inPort, tempMac);
    outAddress->macAddressFromRaw(tempMac, BipAddrLength);
    return BipAddrLength;
}

quint16 BacnetBipAddressHelper::ipAddrToRaw(QHostAddress address, quint16 port, quint8 *outData)
{
    quint32 hostAddr = address.toIPv4Address();
//    HelperCoder::printArray((const quint8*)&hostAddr, sizeof(quint32), "--- Representation:");
    HelperCoder::uint32ToRaw(hostAddr, outData);
    quint8 *portField = &outData[BipIpLength];
//    qDebug()<<"Port was"<<hex<<*(quint8*)&port<<*((quint8*)&port + 1);
    HelperCoder::uin16ToRaw(port, portField);
//    qDebug()<<"Port was"<<hex<<*portField<<*(portField + 1);
    HelperCoder::printArray(outData, BacnetBipAddressHelper::BipAddrLength, "--- Repres. after:");

    return BipAddrLength;
}

//#define BIP_ADDR_TEST
#ifdef BIP_ADDR_TEST

#include <QDebug>

int main()
{
    QHostAddress ipAddr("192.168.2.107");
    quint64 port(47808);

    BacnetAddress addr;
    BacnetBipAddressHelper::setMacAddress(ipAddr, port, &addr);


    qDebug() << "Was"<<ipAddr<<port<<"is"<<BacnetBipAddressHelper::ipAddress(addr)<<BacnetBipAddressHelper::ipPort(addr);

    Q_ASSERT_X(BacnetBipAddressHelper::ipAddress(addr) == ipAddr, __PRETTY_FUNCTION__, "Addresses mismatch!");
    Q_ASSERT_X(BacnetBipAddressHelper::ipPort(addr) == port, __PRETTY_FUNCTION__, "Ports mismatch!");

    return 0;
}

#endif //BIP_ADDR_TEST
