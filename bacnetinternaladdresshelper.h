#ifndef BACNETINTERNALADDRESSHELPER_H
#define BACNETINTERNALADDRESSHELPER_H

#include "bacnetaddress.h"

typedef quint32 InternalAddress;

namespace BacnetInternalAddressHelper
{
    enum {
        InternalAddressLength   = sizeof(InternalAddress),
        InternalMaskLength      = InternalAddressLength,
    };

    const InternalAddress InvalidInternalAddress = (InternalAddress)(-1);

    quint8 macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress);

    InternalAddress internalAddress(quint32 hostOrderAddress);
    InternalAddress internalAddress(const BacnetAddress &inAddress);

    quint16 internalAddrToRaw(InternalAddress address, quint8 *outData);

    BacnetAddress toBacnetAddress(InternalAddress address);
};

#endif // BACNETINTERNALADDRESSHELPER_H
