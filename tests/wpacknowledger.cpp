#include "wpacknowledger.h"

namespace Bacnet {

WpAcknowledger::WpAcknowledger(quint8 invokeId, BacnetAddress &answererAddress, BacnetAddress &handlerAddress, BacnetApplicationLayerHandler *appLayerHandler):
    invokeId(invokeId),
    answererAddress(answererAddress),
    handlerAddress(handlerAddress),
    appLayerHandler(appLayerHandler)
{

}

void WpAcknowledger::answer()
{
    quint8 wpAckData[] = {
        0x20,
        invokeId,
        0x0f
    };
    quint32 wpAckDataSize = sizeof(wpAckData);
    appLayerHandler->indication(wpAckData, wpAckDataSize, answererAddress, handlerAddress);
}


} // namespace Bacnet
