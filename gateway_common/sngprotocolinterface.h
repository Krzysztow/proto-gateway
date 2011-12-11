#ifndef GATEWAYAPPLICATION_SNGPROTOCOLINTERFACE_H
#define GATEWAYAPPLICATION_SNGPROTOCOLINTERFACE_H

#include "protocolinterface.h"

namespace GatewayApplicationNS {

class SngProtocolInterface:
        public ProtocolInterface
{
public:
    virtual bool createProtocol(QString &configPath, QString &validation) const;
    virtual void stop();

};

} // namespace GatewayApplication

#endif // GATEWAYAPPLICATION_SNGPROTOCOLINTERFACE_H
