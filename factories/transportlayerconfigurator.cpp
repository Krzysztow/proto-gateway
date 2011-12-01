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

static const char *BacnetBipAddressSeparator = ":";

BacnetTransportLayerHandler *TransportLayerConfigurator::createTransportLayer(BacnetNetworkLayerHandler *netLayer, QDomElement &transportLayCfg)
{
    Q_CHECK_PTR(netLayer);
    Q_ASSERT(!transportLayCfg.isNull());

    QString str = transportLayCfg.attribute(TransportLayerTypeAttr);
    if (str.isEmpty())
        str = BacnetIpAddressValue;//assume default value

    BacnetTransportLayerHandler *tLayer(0);
    if (BacnetIpAddressValue == str) {
        tLayer = createBipTransportLayer(netLayer, transportLayCfg);
    } else {
        elementError(transportLayCfg, TransportLayerTypeAttr);
    }

    return tLayer;
}

BacnetBipTransportLayer *TransportLayerConfigurator::createBipTransportLayer(BacnetNetworkLayerHandler *netLayer, QDomElement &bipLayCfg)
{
    QString addrStr = bipLayCfg.attribute(BacnetAddressAttribute);
    if (addrStr.isEmpty()) {
        elementError(bipLayCfg, BacnetAddressAttribute, "No address provided!");
        return 0;
    }

    QString addressType = bipLayCfg.attribute(BacnetAddressTypeAttr);

    QHostAddress ipAddress;
    quint16 port(0);
    if (BacnetRawAddressValue == addressType) {
        //expected format is xx:xx:xx:xx:pp:pp, all numbers in hexadecimal
        BacnetAddress address;
        if (!address.macAddressFromString(addrStr)) {
            elementError(bipLayCfg, BacnetAddressTypeAttr);
            return 0;
        }
        ipAddress = BacnetBipAddressHelper::ipAddress(address);
    } else { //assume default is ip-like address
        //expected format is xxx.xxx.xxx.xxx:<port-num>, all numbers in decimal
        QStringList addrList = addrStr.split(BacnetBipAddressSeparator);
        if (addrList.count() == 0) {
            elementError(bipLayCfg, BacnetAddressTypeAttr);
            return 0;
        }
        ipAddress.setAddress(addrList.first());
        bool ok(false);
        if (addrList.count() == 2) {
            port = addrList.at(1).toUInt(&ok);
        }
        if (!ok)
            port = 0xBAC0;//assume default port
    }

    if (ipAddress.isNull()) {
        qDebug("%s : Can't parse address.", __PRETTY_FUNCTION__);
        return 0;
    }

    BacnetBipTransportLayer *bip = new BacnetBipTransportLayer(netLayer);
    bip->transportLayer()->setAddress(ipAddress, port);

    return bip;
}
