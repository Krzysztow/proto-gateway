#ifndef BACNET_COVANSWERER_H
#define BACNET_COVANSWERER_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetapplicationlayer.h"

namespace Bacnet {

class CovAnswerer:
        public QObject
{
    Q_OBJECT;
public:
    CovAnswerer(quint8 invokeId, BacnetAddress &answererAddress, BacnetAddress &handlerAddress, BacnetApplicationLayerHandler *appLayerHandler);

public slots:
    void answer();

private:
    quint8 invokeId;
    BacnetAddress &answererAddress;
    BacnetAddress &handlerAddress;
    BacnetApplicationLayerHandler *appLayerHandler;

    enum States {
        ToSendCovSubscrAck,
        ToSendConfirmedCovNotification,
        ToSendUnconfirmedNotification,
        FINISHED
    } _state;
};


} // namespace Bacnet

#endif // BACNET_COVANSWERER_H
