#include "bacnetprotocolinterface.h"

#include <QFile>
#include <QHash>

#include "bacnetvirtuallinklayer.h"
#include "bacnetudptransportlayer.h"
#include "bacnetbiptransportlayer.h"
#include "bacnetnetworklayer.h"
#include "bacnetapplicationlayer.h"

#include "networklayerconfigurator.h"
#include "transportlayerconfigurator.h"
#include "bacnetconfigurator.h" // configures application layer only

using namespace GatewayApplicationNS;

static const char *TransportLayerTag    = "transportLayer";
static const char *NetworkLayerTag      = "networkLayer";
static const char *AppLayerTag          = "appLayer";

bool BacnetProtocolInterface::createProtocol(QString &configPath, QString &validation) const
{
    QFile f(configPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug("Can't open a config file %s!", qPrintable(configPath));
        return false;
    }
    QDomDocument doc;
    if (!doc.setContent(&f)) {
        qDebug("Main config (%s) is malformed", qPrintable(configPath));
        return false;
    }

    QDomElement mainElement = doc.documentElement();
    QDomElement element;

    element = mainElement.firstChildElement(TransportLayerTag);
    QHash<quint8, BacnetTransportLayerHandler*> createdPorts = Bacnet::TransportLayerConfigurator::createTransportLayer(element);
    if (createdPorts.count() == 0) {
        qDebug("Transport layer was not created, terminate!");
        return false;
    }


    //configure network layer
    element = mainElement.firstChildElement(NetworkLayerTag);
    BacnetNetworkLayerHandler *networkLayer = Bacnet::NetworkLayerConfigurator::createNetworkLayer(createdPorts, element);
    Q_CHECK_PTR(networkLayer);
    if (0 == networkLayer) {
        qDebug("Network layer was not created, terminate BACnet module!");
        qDeleteAll(createdPorts);
        return false;
    }


    //set virtual network parameters.
    element = mainElement.firstChildElement(AppLayerTag);
    Bacnet::BacnetApplicationLayerHandler *appLayer = Bacnet::BacnetConfigurator::createApplicationLayer(networkLayer, element);
    if (0 == appLayer) {
        qDebug("Application layer was not created, terminate BACnet module!");

        qDeleteAll(createdPorts);
        delete networkLayer;
        return false;
    }

    Q_CHECK_PTR(appLayer);

    return true;
}

void BacnetProtocolInterface::stop()
{
    qDebug("%s : not implemented!", __PRETTY_FUNCTION__);
    Q_ASSERT(false);
}
