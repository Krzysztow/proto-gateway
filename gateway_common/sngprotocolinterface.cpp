#include "sngprotocolinterface.h"

using namespace GatewayApplicationNS;

#include "sngfactory.h"
#include "connectionmanager.h"

static const char SngPropertiesTagName[] = "sngProperties";

bool SngProtocolInterface::createProtocol(QString &configPath, QString &validation) const
{
    QFile f(configPath);
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

void SngProtocolInterface::stop()
{
    qDebug("%s : Not implemented!", __PRETTY_FUNCTION__);
    Q_ASSERT(false);
}
