#include "servicefactory.h"

#include "internalwprequesthandler.h"
#include "internalrprequesthandler.h"
#include "internalwhoisrequesthandler.h"
#include "internalwhohasrequesthandler.h"
#include "internalsubscribecovrequesthandler.h"
#include "bacnetpci.h"

::InternalConfirmedRequestHandler *ServiceFactory::createConfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                                          ::BacnetConfirmedRequestData *pciData,
                                                                          Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                                          Bacnet::BacnetApplicationLayerHandler *appLayer)
{    
    Q_CHECK_PTR(pciData);
    Q_CHECK_PTR(tsm);
    Q_CHECK_PTR(appLayer);

    switch (pciData->service())
    {
    case (BacnetServicesNS::WriteProperty) :
    {
        return new Bacnet::InternalWPRequestHandler(pciData, requester, destination, tsm, device, appLayer);;
    }
    case (BacnetServicesNS::ReadProperty) :
    {
        return new Bacnet::InternalRPRequestHandler(pciData, requester, destination, tsm, device, appLayer);
    }
    case (BacnetServicesNS::SubscribeCOV)://fall through
    case (BacnetServicesNS::SubscribeCOVProperty):
    {
        return new Bacnet::InternalSubscribeCOVRequestHandler(pciData, requester, destination, tsm, device, appLayer);
    }
    default:
        Q_ASSERT(false);
        return 0;//should
    }
}

::InternalUnconfirmedRequestHandler *ServiceFactory::createUnconfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                                              BacnetUnconfirmedRequestData &pciData,
                                                                              Bacnet::BacnetTSM2 *tsm, Bacnet::BacnetDeviceObject *device,
                                                                              Bacnet::BacnetApplicationLayerHandler *appLayer)
{
    Q_CHECK_PTR(appLayer);
    Q_UNUSED(destination);
    Q_UNUSED(pciData);
    switch (pciData.service())
    {
    case (BacnetServicesNS::WhoIs):
    {
        return new Bacnet::InternalWhoIsRequestHandler(requester, tsm, device, appLayer);
    }
    case (BacnetServicesNS::WhoHas):
    {
        return new Bacnet::InternalWhoHasRequestHandler(requester, tsm, device, appLayer);
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
