#include "servicefactory.h"

#include "internalwprequesthandler.h"
#include "internalrprequesthandler.h"
#include "internalwhoisrequesthandler.h"
#include "internalwhohasrequesthandler.h"
#include "bacnetpci.h"

::InternalConfirmedRequestHandler *ServiceFactory::createConfirmedHandler(::BacnetConfirmedRequestData *pciData,
                                                      Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                      InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler)
{
    Q_CHECK_PTR(pciData);
    Q_CHECK_PTR(tsm);
    Q_CHECK_PTR(internalHandler);
    Q_CHECK_PTR(externalHandler);

    switch (pciData->service())
    {
    case (BacnetServices::WriteProperty) :
        {
            Bacnet::InternalWPRequestHandler *wpService = new Bacnet::InternalWPRequestHandler(tsm, device, internalHandler, externalHandler);
            wpService->setConfirmedData(pciData);
            return wpService;
        }
    case (BacnetServices::ReadProperty) :
        {
            Bacnet::InternalRPRequestHandler *rpService = new Bacnet::InternalRPRequestHandler(tsm, device, internalHandler, externalHandler);
            rpService->setConfirmedData(pciData);
            return rpService;
        }
    default:
        Q_ASSERT(false);
        return 0;//should
    }
}

::InternalUnconfirmedRequestHandler *ServiceFactory::createUnconfirmedHandler(BacnetUnconfirmedRequestData *pciData,
                                                                                    Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                                                    InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler)
{
    Q_CHECK_PTR(pciData);
    switch (pciData->service())
    {
    case (BacnetServices::WhoIs):
        {
            return new Bacnet::InternalWhoIsRequestHandler(tsm, device, internalHandler, externalHandler);
        }
    case (BacnetServices::WhoHas):
        {
            return new Bacnet::InternalWhoHasRequestHandler(tsm, device, internalHandler, externalHandler);
        }
    default:
        Q_ASSERT(false);
        return 0;
    }
}

//BacnetService *ServiceFactory::createBroadcastService(quint8 *servicePtr, quint16 length,
//                                                      quint8 serviceCode, qint32 *retCode)
//{
//    Q_CHECK_PTR(retCode);
//    Q_CHECK_PTR(servicePtr);

//    switch (serviceCode)
//    {
//    case (BacnetServices::I_Am):
//        ;
//    }
//}
