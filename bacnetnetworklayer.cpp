#include "bacnetnetworklayer.h"
#include "bacnettransportlayer.h"
#include "bacnetapplicationlayer.h"

#define REAL_APP_LAYER_NUM -1

BacnetNetworkLayerHandler::BacnetNetworkLayerHandler()
{
}

void BacnetNetworkLayerHandler::readNpdu(quint8 *npdu, quint16 length, BacnetAddress &dlSrcAddress)
{
    //check if we can handle it - this class implements only de/en-coding of BACnet protocol version 1
    quint8 *actualBytePtr = npdu;
    if (*actualBytePtr != ProtocolVersion) {
#warning "What to do here? Just drop the frame, or send something back."
        return;
    }

    //now it points at the NPCI field
    actualBytePtr++;
    Q_ASSERT(NpciFieldHelper::isNpciSane(actualBytePtr));
    //if its a network message
    if (NpciFieldHelper::isNetworkLayerMessage(actualBytePtr)) {

    } else {

    }
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
