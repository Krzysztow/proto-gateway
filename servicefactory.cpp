#include "servicefactory.h"

#include "internalwprequesthandler.h"
#include "internalrprequesthandler.h"
#include "internalwhoisrequesthandler.h"
#include "internalwhohasrequesthandler.h"
#include "internalsubscribecovrequesthandler.h"
#include "internalihaveservicehandler.h"
#include "internaliamservicehandler.h"
#include "internalconfirmedcovnotifhandler.h"
#include "bacnetpci.h"

::InternalConfirmedRequestHandler *ServiceFactory::createConfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                                          ::BacnetConfirmedRequestData *pciData, Bacnet::BacnetDeviceObject *device,
                                                                          Bacnet::BacnetApplicationLayerHandler *appLayer)
{    
    Q_CHECK_PTR(pciData);
    Q_CHECK_PTR(appLayer);

    switch (pciData->service())
    {
    case (BacnetServicesNS::WriteProperty) : {
        return new Bacnet::InternalWPRequestHandler(pciData, requester, destination, device, appLayer);;
    }
    case (BacnetServicesNS::ReadProperty) : {
        return new Bacnet::InternalRPRequestHandler(pciData, requester, destination, device, appLayer);
    }
    case (BacnetServicesNS::SubscribeCOV)://fall through
    case (BacnetServicesNS::SubscribeCOVProperty):
    {
        return new Bacnet::InternalSubscribeCOVRequestHandler(pciData, requester, destination, device, appLayer);
    }
    case (BacnetServicesNS::ConfirmedCOVNotification):
    {
        return new Bacnet::InternalConfirmedCovNotifHandler(pciData, requester, destination, appLayer);
    }
    default:
        Q_ASSERT(false);
        return 0;//should
    }
}

Bacnet::InternalUnconfirmedRequestHandler *ServiceFactory::createUnconfirmedHandler(BacnetAddress &requester, BacnetAddress &destination,
                                                                              BacnetUnconfirmedRequestData &pciData, Bacnet::BacnetDeviceObject *device,
                                                                              Bacnet::BacnetApplicationLayerHandler *appLayer)
{
    Q_CHECK_PTR(appLayer);
    Q_UNUSED(destination);
    Q_UNUSED(pciData);
    qDebug("=== address lenght is %d", requester.macAddrLength());
    switch (pciData.service())
    {
    case (BacnetServicesNS::WhoIs): {
        return new Bacnet::InternalWhoIsRequestHandler(requester, device, appLayer);
    }
    case (BacnetServicesNS::WhoHas): {
        return new Bacnet::InternalWhoHasRequestHandler(requester, device, appLayer);
    }
    case (BacnetServicesNS::I_Have): {
        return new Bacnet::InternalIHaveServiceHandler(requester, appLayer);
    }
    case (BacnetServicesNS::I_Am): {
        return new Bacnet::InternalIAmServiceHandler(appLayer, requester);
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
