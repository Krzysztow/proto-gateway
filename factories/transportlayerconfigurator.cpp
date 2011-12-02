#include "transportlayerconfigurator.h"

#include <QStringList>

#include "bacnettransportlayer.h"
#include "bacnetbiptransportlayer.h"
#include "bacnetaddress.h"

#include "configuratorhelper.h"

using namespace Bacnet;

static const char *BacnetAddressTypeAttr    = "bac-addr-type";
static const char *BacnetRawAddressValue    = "raw";
static const char *BacnetIpAddressValue     = "bip";
static const char *TransportLayerTypeAttr   = "type";
static const char *BacnetAddressAttribute   = "address";
static const char *PortTagName              = "port";
static const char *PortIdAttribute          = "port-id";

QHash<quint8, BacnetTransportLayerHandler*> TransportLayerConfigurator::createTransportLayer(QDomElement &transportLayCfg)
{
    Q_ASSERT(!transportLayCfg.isNull());

    QHash<quint8, BacnetTransportLayerHandler*> createdPorts;
    bool ok;
    quint8 portId;
    for (QDomElement portElement = transportLayCfg.firstChildElement(PortTagName); !portElement.isNull(); portElement = portElement.nextSiblingElement(PortTagName)) {
        //get port id
        portId = portElement.attribute(PortIdAttribute).toUInt(&ok);
        if (!ok) {
            elementError(portElement, PortIdAttribute);
            //skip
            continue;
        }

        if (createdPorts.contains(portId)) {//we cannot allow for duplications
            qDebug("%s : PortId (%d) is already used.", __PRETTY_FUNCTION__, portId);
            continue;
        }

        QString str = portElement.attribute(TransportLayerTypeAttr);
        if (str.isEmpty())
            str = BacnetIpAddressValue;//assume default value

        BacnetTransportLayerHandler *tLayer(0);
        if (BacnetIpAddressValue == str) {
            tLayer = createBipTransportLayer(portElement);
        } else {
            //there weas an error/. Don;t have to continue or break, since 0 != tLayer takes care of that.
            elementError(portElement, TransportLayerTypeAttr);
        }

        if (0 != tLayer) {
#warning "netLayer->addTransportLayer(network, portId, tLayer); in network layer creation!"
            createdPorts.insert(portId, tLayer);
        }
    }

    return createdPorts;
}

BacnetBipTransportLayer *TransportLayerConfigurator::createBipTransportLayer(QDomElement &bipLayCfg)
{
    QString addrStr = bipLayCfg.attribute(BacnetAddressAttribute);
    if (addrStr.isEmpty()) {
        elementError(bipLayCfg, BacnetAddressAttribute, "No address provided!");
        return 0;
    }

    QString addressType = bipLayCfg.attribute(BacnetAddressTypeAttr);

    QHostAddress ipAddress;
    quint64 port(0);
    if (BacnetRawAddressValue == addressType) {
        //expected format is xx:xx:xx:xx:pp:pp, all numbers in hexadecimal
        BacnetAddress address;
        if (!address.macAddressFromString(addrStr)) {
            elementError(bipLayCfg, BacnetAddressTypeAttr);
            return 0;
        }
        ipAddress = BacnetBipAddressHelper::ipAddress(address);
        port = BacnetBipAddressHelper::ipPort(address);
    } else { //assume default is ip-like address
        //expected format is xxx.xxx.xxx.xxx:<port-num>, all numbers in decimal

        if (!BacnetBipAddressHelper::macAddressFromString(addrStr, &ipAddress, &port)) {
            elementError(bipLayCfg, BacnetAddressTypeAttr);
            return 0;
        }
    }

    if ( ipAddress.isNull() || (0 == port) ) {
        qDebug("%s : Can't parse address.", __PRETTY_FUNCTION__);
        return 0;
    }

    BacnetBipTransportLayer *bip = new BacnetBipTransportLayer();
    bip->transportLayer()->setAddress(ipAddress, port);    

    return bip;
}
