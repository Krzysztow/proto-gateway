#ifndef PROTOCOLINTERFACE_H
#define PROTOCOLINTERFACE_H

#include <QDomDocument>

namespace GatewayApplicationNS {

class ProtocolInterface
{
public:
    virtual bool createProtocol(QString &configPath, QString &validation) const = 0;
    virtual void stop() = 0;
};

}

#endif // PROTOCOLINTERFACE_H
