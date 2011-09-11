#include "servicefactory.h"

#include "bacnetwritepropertyservice.h"
#include "bacnetreadpropertyservice.h"

BacnetService *ServiceFactory::createService(quint8 *servicePtr, quint16 length,
                                             quint8 serviceCode, qint32 *retCode)
{
    Q_CHECK_PTR(retCode);

    switch (serviceCode)
    {
    case (BacnetServices::WriteProperty) :
        {
            BacnetWritePropertyService *wpService = new BacnetWritePropertyService();
            *retCode = wpService->fromRaw(servicePtr, length);
            return wpService;
        }
    case (BacnetServices::ReadProperty) :
        {
            BacnetReadPropertyService *rpService = new BacnetReadPropertyService();
            *retCode = rpService->fromRaw(servicePtr, length);
            return rpService;
        }
    default:
        Q_ASSERT(false);
        return 0;//should
    }
}

