#include "internalunconfirmedrequesthandler.h"

#include "bacnetpci.h"

InternalUnconfirmedRequestHandler::InternalUnconfirmedRequestHandler(/*Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                                     InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler*/)
{
}

InternalUnconfirmedRequestHandler::~InternalUnconfirmedRequestHandler()
{

}

void InternalUnconfirmedRequestHandler::setUnconfirmedData(BacnetUnconfirmedRequestData *reqData)
{
    delete _reqData;
    _reqData = reqData;
}

void InternalUnconfirmedRequestHandler::setAddresses(BacnetAddress &requester, BacnetAddress &destination)
{
    _requester = requester;
    _destination = destination;
}

//bool InternalUnconfirmedRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
//{

//}

//bool InternalUnconfirmedRequestHandler::isFinished()
//{

//}

//void InternalUnconfirmedRequestHandler::finalize(bool *deleteAfter)
//{

//}

//QList<int> InternalUnconfirmedRequestHandler::execute()
//{

//}
