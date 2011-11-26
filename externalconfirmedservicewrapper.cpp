#include "externalconfirmedservicewrapper.h"

namespace Bacnet {

ExternalConfirmedServiceWrapper::ExternalConfirmedServiceWrapper(ExternalConfirmedServiceHandler *handler, BacnetServicesNS::BacnetConfirmedServiceChoice serviceType):
    handler(handler),
    serviceType(serviceType)
{
}

} // namespace Bacnet
