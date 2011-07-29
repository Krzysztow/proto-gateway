#include "bacnetnetworklayer.h"
#include "bacnettransportlayer.h"
#include "bacnetapplicationlayer.h"
#include "bacnetnpci.h"

#define REAL_APP_LAYER_NUM -1

BacnetNetworkLayerHandler::BacnetNetworkLayerHandler()
{
}

void BacnetNetworkLayerHandler::readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress)
{
    //check if we can handle it - this class implements only de/en-coding of BACnet protocol version 1
    quint8 *actualBytePtr = npdu;
    qint32 ret;

    //now it points at the NPCI field
    actualBytePtr++;
    BacnetNpci npci;
    ret = npci.setFromRaw(actualBytePtr);
    Q_ASSERT(ret < 0);
    Q_ASSERT(npci.isSane());
    if (ret < 0) {
        return;
    }

    actualBytePtr += ret;

}

void BacnetNetworkLayerHandler::setTransportLayer(BacnetTransportLayerHandler *transportHndlr)
{
    //first make sure they are different. Otherwise we would be calling freed resources.
    if (_transportHndlr != transportHndlr) {
        delete _transportHndlr;
        _transportHndlr = transportHndlr;
    }
}

void BacnetNetworkLayerHandler::setApplicationLayer(BacnetApplicationLayerHandler *appHndlr)
{
    //they all are stored in a same fashion
    setVirtualApplicationLayer(REAL_APP_LAYER_NUM, appHndlr);
}

void BacnetNetworkLayerHandler::setVirtualApplicationLayer(quint16 virtualNetworkNum, BacnetApplicationLayerHandler *appHndlr)
{
    //we don't care if the application layer is null.
    if (_networks.contains(virtualNetworkNum)) {
        BacnetApplicationLayerHandler *oldApp = _networks.value(virtualNetworkNum);
        if (oldApp != appHndlr) {
            delete oldApp;
        }
    }
    //even if it existed and was the same, nothing bad happens.
    _networks.insert(virtualNetworkNum, appHndlr);
    if (0 != appHndlr) {
        appHndlr->setNetworkHandler(this);
    }
}
