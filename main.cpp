#include <QtCore/QCoreApplication>

#include "bacnetvirtuallinklayer.h"
#include "bacnetudptransportlayer.h"
#include "bacnetbiptransportlayer.h"
#include "bacnetnetworklayer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BacnetNetworkLayerHandler *networkLayer = new BacnetNetworkLayerHandler();
    networkLayer->setVirtualApplicationLayer(2, 0);
    BacnetBipTransportLayer *transportBipLayer = new BacnetBipTransportLayer(networkLayer);
    Q_UNUSED(transportBipLayer);
    qDebug()<<"Bound to adderss"<<transportBipLayer->transportLayer()->setAddress(QHostAddress("192.168.2.107"), 0xBAC0);

    return a.exec();
}
