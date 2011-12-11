#ifndef GATEWAYAPPLICATIONNS_PROTOCOLSMANAGER_H
#define GATEWAYAPPLICATIONNS_PROTOCOLSMANAGER_H

#include <QString>
#include <QMap>

namespace GatewayApplicationNS {

class ProtocolInterface;
class ProtocolsManager
{
public:
    ProtocolsManager(QString configPath);
    void init();

private:
    ProtocolInterface *createProtocol(QString protocolName, QString &protocolConfigPath, QString &validation);

private:
    QString _configPath;
    QMap<QString, ProtocolInterface*> _loadedProtocols;
};

} // namespace GatewayApplicationNS

#endif // GATEWAYAPPLICATIONNS_PROTOCOLSMANAGER_H
