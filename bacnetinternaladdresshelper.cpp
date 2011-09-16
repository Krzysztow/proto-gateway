#include "bacnetinternaladdresshelper.h"

#include <QtEndian>

#include "helpercoder.h"

using namespace BacnetInternalAddressHelper;

quint8 BacnetInternalAddressHelper::macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress)
{
    Q_ASSERT(0 != outAddress);
    outAddress->macAddressFromRaw(addrRawPtr, InternalAddressLength);
    //return number of bytes consumed
    return InternalAddressLength;
}


InternalAddress BacnetInternalAddressHelper::internalAddress(const BacnetAddress &inAddress)
{
    Q_ASSERT(inAddress.macAddrLength() == (InternalAddressLength));
    if (inAddress.macAddrLength() == InternalAddressLength) {
        return qFromBigEndian(*(quint32*)inAddress.macPtr());
    }
    else return InvalidInternalAddress;
}

quint16 BacnetInternalAddressHelper::internalAddrToRaw(InternalAddress address, quint8 *outData)
{
    return HelperCoder::uint32ToRaw(address, outData);
}
