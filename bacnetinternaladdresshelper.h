#ifndef BACNETINTERNALADDRESSHELPER_H
#define BACNETINTERNALADDRESSHELPER_H

#include "bacnetaddress.h"

typedef quint32 InternalAddress;

namespace BacnetInternalAddressHelper
{
    enum {
        InternalAddressLength   = 4,
        InternalMaskLength      = InternalAddressLength,
    };

    const InternalAddress InvalidInternalAddress = (InternalAddress)(-1);

    quint8 macAddressFromRaw(quint8 *addrRawPtr, BacnetAddress *outAddress);

    InternalAddress internalAddress(const BacnetAddress &inAddress);

    quint16 internalAddrToRaw(InternalAddress address, quint8 *outData);
};

#endif // BACNETINTERNALADDRESSHELPER_H
