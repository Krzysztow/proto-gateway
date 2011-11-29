#include "rpanswerer.h"

using namespace Bacnet;

RpAnswerer::RpAnswerer(quint8 invokeId, BacnetAddress &answererAddress, BacnetAddress &handlerAddress, BacnetApplicationLayerHandler *appLayerHandler):
    invokeId(invokeId),
    answererAddress(answererAddress),
    handlerAddress(handlerAddress),
    appLayerHandler(appLayerHandler)
{

}

void Bacnet::RpAnswerer::answer()
{
//    quint8 rpServiceData[] = {
//        0x00,
//        0x00,
//        0x01,
//        0x0c,
//        0x0c,
//        0x00, 0x00, 0x00, 0x05,
//        0x19,
//        0x55
//    };

    quint8 rpServiceData[] = {
        0x30,
        invokeId,
        0x0c,
        0x0c,
        0x00, 0x00, 0x00, 0x05,
        0x19,
        0x55,
        0x3e,
        0x44,
        0x42, 0x90, 0x99, 0x9a,
        0x3f
    };
    quint32 rpServiceDataSize = sizeof(rpServiceData);
    appLayerHandler->indication(rpServiceData, rpServiceDataSize, answererAddress, handlerAddress);
}
