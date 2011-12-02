#include "networklayerconfigurator.h"

#include "bacnetnetworklayer.h"
#include "configuratorhelper.h"
#include "bacnetbipaddress.h"
#include "bacnettransportlayer.h"

using namespace Bacnet;

static const char *RoutingTableTag                  = "routingTable";
static const char *RouteElementTag                  = "route";
static const char *DirectNetworkNumberAttribute     = "direct-net-num";
static const char *NetworkNumberAttribute           = "net-num";
static const char *NetworkRouterAddressAttr         = "net-router-address";
static const char *PortIdAttribute                  = "port-id";
static const char *ReachableNetworkTag              = "reachableNet";
static const char *BacnetAddrTypeAttr               = "bac-addr-type";
static const char *BacnetAddrRawTypeValue           = "raw";
static const char *BacnetAddrBipTypeValue           = "ip";

BacnetNetworkLayerHandler *NetworkLayerConfigurator::createNetworkLayer(QHash<quint8, BacnetTransportLayerHandler*> &ports, QDomElement &netCfgElement)
{
    Q_ASSERT(!netCfgElement.isNull());
    BacnetNetworkLayerHandler *netLayer = new ::BacnetNetworkLayerHandler();
    netLayer->addPorts(ports);

    QList<quint16> networksUsed;
    QDomElement rTableElement = netCfgElement.firstChildElement(RoutingTableTag);

    quint8 portId;
    quint16 network;
    bool ok;

    QDomElement routeElement;
    QDomElement routeTabElem;

    for(routeElement = rTableElement.firstChildElement(RouteElementTag); !routeElement.isNull(); routeElement = routeElement.nextSiblingElement(RouteElementTag)) {
        //get port id - required
        portId = routeElement.attribute(PortIdAttribute).toUInt(&ok);
        if (!ok) {
            elementError(routeElement, PortIdAttribute);
            //discard, it's important
            continue;
        }
        //get network number - so far required, maybe later not
        network = routeElement.attribute(DirectNetworkNumberAttribute).toUInt(&ok);
        if (!ok) {
            elementError(routeElement, DirectNetworkNumberAttribute);
            continue;
        }
        //check if network is not set to be accessible from some other port. Check if port was created and is not used.
        if (networksUsed.contains(network) || !ports.contains(portId)) {
            elementError(routeElement, "", "Network already accessible or port already used");
            continue;
        }

        //add direct network
        netLayer->setPortDirectNetwork(portId, network);
        networksUsed.append(network);
        //add static routing information
        QHostAddress routerAddress;
        quint64 routerPort;
        BacnetAddress address;
        QString str;
        QString addrType;
        for (routeTabElem = routeElement.firstChildElement(ReachableNetworkTag); !routeTabElem.isNull(); routeTabElem = routeTabElem.nextSiblingElement(ReachableNetworkTag)) {
            //get network number.
            network = routeTabElem.attribute(NetworkNumberAttribute).toUInt(&ok);
            if (!ok) {
                elementError(routeTabElem, NetworkNumberAttribute);
                continue;
            }
            //get router address
            addrType = routeTabElem.attribute(BacnetAddrTypeAttr);
            str = routeTabElem.attribute(NetworkRouterAddressAttr);
            if (BacnetAddrRawTypeValue == addrType) {
                ok = address.macAddressFromString(str);
            } else {
                Q_UNUSED(BacnetAddrBipTypeValue);
                ok = BacnetBipAddressHelper::macAddressFromString(str, &routerAddress, &routerPort);
                BacnetBipAddressHelper::setMacAddress(routerAddress, routerPort, &address);
            }

            if (!ok) {
                elementError(routeElement, NetworkRouterAddressAttr);
                continue;
            }

            //ensure we don't already have a route to this address
            if (networksUsed.contains(network)) {
                qDebug("%s : There can't be more than one route to network (%s)", __PRETTY_FUNCTION__, qPrintable(elementString(routeTabElem)));
                continue;
            }
            netLayer->updateRoutingTableIndirectAccess(portId, QVector<quint16>() << network, address);
            networksUsed.append(network);
        }
    }

    return netLayer;
}
