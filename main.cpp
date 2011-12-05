#include <QtCore/QCoreApplication>
#include <QDomDocument>

#include "bacnetvirtuallinklayer.h"
#include "bacnetudptransportlayer.h"
#include "bacnetbiptransportlayer.h"
#include "bacnetnetworklayer.h"
#include "bacnetapplicationlayer.h"

#include "networklayerconfigurator.h"
#include "transportlayerconfigurator.h"
#include "bacnetconfigurator.h" // configures application layer only

#include "cdm.h"
#include "sngfactory.h"

#define MAIN_BACNET
#ifdef MAIN_BACNET

static const char *MainConfigFile       = "bacnet-test-config.xml";

static const char *TransportLayerTag    = "transportLayer";
static const char *NetworkLayerTag      = "networkLayer";
static const char *AppLayerTag          = "appLayer";

bool createBacnetModule()
{
    QFile f(MainConfigFile);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug("Can't open a config file %s!", MainConfigFile);
        return false;
    }
    QDomDocument doc;
    if (!doc.setContent(&f)) {
        qDebug("Main config (%s) is malformed", MainConfigFile);
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

#include "connectionmanager.h"

static const char SngPropertiesTagName[]            = "sngProperties";

bool createSngModule()
{
    QFile f("sng-test-config.xml");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Can't open the configuration file.");
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(&f)) {
        qDebug("Malformed sng config!");
        return false;
    }

    ConnectionManager::instance()->configChanged(doc);
    QDomElement el = doc.documentElement().firstChildElement(SngPropertiesTagName);

    Sng::SngHandler *handler = Sng::SngFactory::createModule(el);

    if (0 == handler)
        return false;

    return true;
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataModel::instance()->startFactory();  //it contains Property instances, so we start DataModel

    bool ok = createBacnetModule();
    if (!ok)
        return 1;

    ok = createSngModule();
    if (!ok)
        return 2;

    DataModel::instance()->stopFactory();

    return a.exec();
}
#endif
