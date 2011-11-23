#include "internalunconfirmedrequesthandler.h"

#include "bacnetpci.h"

InternalUnconfirmedRequestHandler::InternalUnconfirmedRequestHandler(BacnetUnconfirmedRequestData *reqData):
    _reqData(reqData)
{
    Q_CHECK_PTR(_reqData);
}

InternalUnconfirmedRequestHandler::~InternalUnconfirmedRequestHandler()
{
    delete _reqData;
    _reqData = 0;
}

