#include "covanswerer.h"

namespace Bacnet {

CovAnswerer::CovAnswerer(quint8 invokeId, BacnetAddress &answererAddress, BacnetAddress &handlerAddress, BacnetApplicationLayerHandler *appLayerHandler):
    invokeId(invokeId),
    answererAddress(answererAddress),
    handlerAddress(handlerAddress),
    appLayerHandler(appLayerHandler),
    _state(ToSendCovSubscrAck)
{
}

void CovAnswerer::answer() {

    if (ToSendCovSubscrAck == _state) {

        //COV-Property Response
        quint8 covRespData[] = {
            0x20,
            invokeId,
            0x1c
        };
        const int covRespDataSize = sizeof(covRespData);
        appLayerHandler->indication(&covRespData[0], covRespDataSize, answererAddress, handlerAddress);

    } else if (ToSendConfirmedCovNotification == _state) {
        quint8 covConfirmedNotifData[] = {
            0x00,
            0x02,
            0x0f,
            0x01,
            0x09,
            0x12,
            0x1c,
            0x02, 0x00, 0x00, 0x04,
            0x2c,
            0x00, 0x00, 0x00, 0x0a,
            0x39,
            0x00,
            0x4e,
            0x09,
            0x55,
            0x2e,
            0x44,
            0x42, 0x82, 0x00, 0x00,
            0x2f,
            0x09,
            0x6f,
            0x2e,
            0x82,
            0x04, 0x00,
            0x2f,
            0x4f
        };
        const quint32 covConfirmedNotifDataSize = sizeof (covConfirmedNotifData);
        appLayerHandler->indication(&covConfirmedNotifData[0], covConfirmedNotifDataSize, answererAddress, handlerAddress);
    } else if (ToSendUnconfirmedNotification == _state) {
        quint8 covUcfrmdNotifData[] = {
            0x10,
            0x02,
            0x09,
            0x12,
            0x1c,
            0x02, 0x00, 0x00, 0x04,
            0x2c,
            0x00, 0x00, 0x00, 0x0a,
            0x39,
            0x00,
            0x4e,
            0x09,
            0x55,
            0x2e,
            0x44,
            0x42, 0x82, 0x00, 0x00,
            0x2f,
            0x09,
            0x6f,
            0x2e,
            0x82,
            0x04, 0x00,
            0x2f,
            0x4f
        };
        const quint32 covUcfrmdNotifDataSize = sizeof (covUcfrmdNotifData);
        appLayerHandler->indication(covUcfrmdNotifData, covUcfrmdNotifDataSize, answererAddress, handlerAddress);
    } else if (FINISHED == _state) {
        return;
    }
    _state = (States)(_state + 1);
}

} // namespace Bacnet
