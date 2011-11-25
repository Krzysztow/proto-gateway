#include "internalconfirmedrequesthandler.h"

#include "bacnetaddress.h"
#include "bacnetobject.h"
#include "bacnetdeviceobject.h"
#include "bacnetservicedata.h"
#include "bacnetpci.h"
#include "error.h"
#include "asynchronousbacnettsmaction.h"
#include "bacnetapplicationlayer.h"

InternalConfirmedRequestHandler::InternalConfirmedRequestHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination):
    _reqData(crData),
    _requester(requester),
    _destination(destination)
{
}


InternalConfirmedRequestHandler::~InternalConfirmedRequestHandler()
{
    delete _reqData;
    _reqData = 0;
}

//void InternalConfirmedRequestHandler::setAddresses(BacnetAddress &requester, BacnetAddress &destination)
//{
//    _requester = requester;
//    _destination = destination;
//}

//void InternalConfirmedRequestHandler::setConfirmedData(BacnetConfirmedRequestData *reqData)
//{
//    delete _reqData;
//    _reqData = reqData;
//}

void InternalConfirmedRequestHandler::finalizeInstant(Bacnet::BacnetApplicationLayerHandler *appLayer)
{
    Q_CHECK_PTR(appLayer);
    Bacnet::BacnetServiceData *response(0);
    if (hasError()) {
        appLayer->sendError(_requester, _destination, _reqData->invokedId(), _reqData->service(), error());
    } else {
         response = takeResponseData();
         appLayer->sendAck(_requester, _destination, response, _reqData);
    }

    qDebug("Don't forget to delete erquestPci and requestService");
}
