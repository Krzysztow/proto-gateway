#include "bacnetinternaladdresshelper.h"

#include "helpercoder.h"

using namespace BacnetInternalAddressHelper;

quint8 BacnetInternalAddressHelper::macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress)
{
    Q_ASSERT(0 != outAddress);
    outAddress->macAddressFromRaw(addrRawPtr, InternalAddressLength);
    //return number of bytes consumed
    return InternalAddressLength;
}

InternalAddress BacnetInternalAddressHelper::internalAddress(quint32 hostOrderAddress)
{
    return hostOrderAddress;
}

InternalAddress BacnetInternalAddressHelper::internalAddress(const BacnetAddress &inAddress)
{
    Q_ASSERT(inAddress.macAddrLength() == (InternalAddressLength) ||
             inAddress.isGlobalBroadcast() ||
             inAddress.isRemoteBroadcast() ||
             inAddress.isLocalBraodacst());
    if (inAddress.macAddrLength() == InternalAddressLength) {
        Q_ASSERT(InternalAddressLength == sizeof(quint32));
        quint32 internalAddress;
        HelperCoder::uint32FromRaw(inAddress.macPtr(), &internalAddress);
        return internalAddress;
    }
    else return InvalidInternalAddress;
}

quint16 BacnetInternalAddressHelper::internalAddrToRaw(InternalAddress address, quint8 *outData)
{
    return HelperCoder::uint32ToRaw(address, outData);
}

BacnetAddress BacnetInternalAddressHelper::toBacnetAddress(InternalAddress address)
{
    quint8 tmp[InternalAddressLength];
    internalAddrToRaw(address, tmp);
    BacnetAddress retAddr;
    retAddr.macAddressFromRaw(tmp, InternalAddressLength);
    return retAddr;
}
