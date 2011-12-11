#ifndef BACNETPROTOCOLINTERFACE_H
#define BACNETPROTOCOLINTERFACE_H

#include "protocolinterface.h"

namespace GatewayApplicationNS {

class BacnetProtocolInterface:
        public ProtocolInterface
{
public:
    virtual bool createProtocol(QString &configPath, QString &validation) const;
    virtual void stop();
};

}

#endif // BACNETPROTOCOLINTERFACE_H
