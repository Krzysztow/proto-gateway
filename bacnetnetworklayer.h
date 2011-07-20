#ifndef BACNETNETWORKLAYERHANDLER_H
#define BACNETNETWORKLAYERHANDLER_H

#include "bacnetaddress.h"

class BacnetNetworkLayerHandler
{
public:
    BacnetNetworkLayerHandler();

    void readNpdu(quint8 *npdu, quint16 length, BacnetAddress &address);
};

#endif // BACNETNETWORKLAYERHANDLER_H
