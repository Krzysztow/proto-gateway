#ifndef BACNET_EXTERNALCONFIRMEDSERVICEWRAPPER_H
#define BACNET_EXTERNALCONFIRMEDSERVICEWRAPPER_H

#include "bacnetcommon.h"

namespace Bacnet {

class ExternalConfirmedServiceHandler;

class ExternalConfirmedServiceWrapper
{
public:
    ExternalConfirmedServiceWrapper(ExternalConfirmedServiceHandler *handler, BacnetServicesNS::BacnetConfirmedServiceChoice serviceType);

public:
    ExternalConfirmedServiceHandler *handler;
    BacnetServicesNS::BacnetConfirmedServiceChoice serviceType;
};

} // namespace Bacnet

#endif // BACNET_EXTERNALCONFIRMEDSERVICEWRAPPER_H
