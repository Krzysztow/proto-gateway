#include "servicefactory.h"

#include "bacnetwritepropertyservice.h"
#include "bacnetreadpropertyservice.h"
#include "internalwhoisrequesthandler.h"
#include "bacnetpci.h"

BacnetService *ServiceFactory::createService(quint8 *servicePtr, quint16 length,
                                             quint8 serviceCode, qint32 *retCode)
{
    Q_CHECK_PTR(retCode);
    Q_CHECK_PTR(servicePtr);

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
            ReadPropertyServiceHandler *rpService = new ReadPropertyServiceHandler();
            *retCode = rpService->fromRaw(servicePtr, length);
            return rpService;
        }
    default:
        Q_ASSERT(false);
        return 0;//should
    }
}

::InternalUnconfirmedRequestHandler *ServiceFactory::createUnconfirmedHandler(BacnetUnconfirmedRequestData *pciData,
                                                                                    Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                                                    InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler)
{
    Q_CHECK_PTR(pciData);
    switch (pciData->service())
    {
    case (BacnetServices::WhoIs):
        {
            return new Bacnet::InternalWhoIsRequestHandler(tsm, device, internalHandler, externalHandler);
        }
    default:
        Q_ASSERT(false);
        return 0;
    }
}

BacnetService *ServiceFactory::createBroadcastService(quint8 *servicePtr, quint16 length,
                                                      quint8 serviceCode, qint32 *retCode)
{
    Q_CHECK_PTR(retCode);
    Q_CHECK_PTR(servicePtr);

    switch (serviceCode)
    {
    case (BacnetServices::I_Am):
        ;
    }
}
