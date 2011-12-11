#include "protocolsmanager.h"

#include <QDomDocument>
#include <QFile>

using namespace GatewayApplicationNS;

const char ProtocolsListTag[]   = "protocols";
const char ProtocolTagName []   = "protocol";
const char ProtocolNameAttr[]   = "name";
const char ProtocolConfigAttr[] = "config";
const char ProtocolValidationAttr[] = "config-validation";
const char ProtocolVersionAttr[]    = "proto-version";

ProtocolsManager::ProtocolsManager(QString configPath):
    _configPath(configPath)
{
}

void ProtocolsManager::init()
{
    QFile f(_configPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("%s : Cannot open main config file!", __PRETTY_FUNCTION__);
        Q_ASSERT(false);
        return;
    }

    QDomDocument doc;
    QString error;
    int r, c;
    if (!doc.setContent(&f, &error, &r, &c)) {
        qDebug("%s : Cannot parse main config file (row %d, col %d, reason: %s)!", __PRETTY_FUNCTION__, r, c, qPrintable(error));
        Q_ASSERT(false);
        return;
    }

    for (QDomElement protocolElement = doc.documentElement().firstChildElement(ProtocolsListTag).firstChildElement(ProtocolTagName);
         !protocolElement.isNull(); protocolElement = protocolElement.nextSiblingElement(ProtocolTagName)) {

        QString protoName = protocolElement.attribute(ProtocolNameAttr);
        QString configName = protocolElement.attribute(ProtocolConfigAttr);
        QString validation = protocolElement.attribute(ProtocolValidationAttr);

        ProtocolInterface *protocolModule = createProtocol(protoName, configName, validation);

        if (0 != protocolModule) {
            _loadedProtocols.insert(protoName, protocolModule);
        }
    }
}

#include "bacnetprotocolinterface.h"
#include "sngprotocolinterface.h"

ProtocolInterface *ProtocolsManager::createProtocol(QString protocolName, QString &protocolConfigPath, QString &validation)
{
    ProtocolInterface *proto(0);

    protocolName = protocolName.toLower();
    if ("bacnet" == protocolName) {
        proto = new BacnetProtocolInterface();
    } else if ("sng" == protocolName) {
        proto = new SngProtocolInterface();
    } else {
        qDebug("%s : Haven't found appropriate protocol for %s", __PRETTY_FUNCTION__, qPrintable(protocolName));
        return 0;
    }

    //protocl is created
    if (proto->createProtocol(protocolConfigPath, validation))
        return proto;

    qDebug("%s : Problem on creating creating protocol %s", __PRETTY_FUNCTION__, qPrintable(protocolName));
    delete proto;
    return 0;
}
