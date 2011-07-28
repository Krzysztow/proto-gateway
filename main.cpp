#include <QtCore/QCoreApplication>

#include "bacnetvirtuallinklayer.h"
#include "bacnetudptransportlayer.h"
#include "bacnetbiptransportlayer.h"
#include "bacnetnetworklayer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BacnetNetworkLayerHandler *networkLayer = new BacnetNetworkLayerHandler();
    BacnetBipTransportLayer *transportBipLayer = new BacnetBipTransportLayer(networkLayer);

    BacnetUdpTransportLayerHandler *transportLayer = new BacnetUdpTransportLayerHandler();
    BacnetBvllHandler *bvllLayer = new BacnetBvllHandler(networkLayer, transportLayer);
    transportLayer->setAddress(QHostAddress("192.168.2.107"), 0xBAC0);

    return a.exec();
}
