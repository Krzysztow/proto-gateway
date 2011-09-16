#include "asynchronousconfirmedhandler.h"

#include "bacnetaddress.h"
#include "bacnetobject.h"
#include "bacnetdeviceobject.h"
#include "bacnetpci.h"
#include "bacnetreadpropertyservice.h"
#include "error.h"
#include "bacneterrorack.h"
#include "asynchronousbacnettsmaction.h"
#include "bacnettsm2.h"

InternalConfirmedRequestHandler::InternalConfirmedRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                                 InternalObjectsHandler *internalHandler, Bacnet::ExternalObjectsHandler *externalHandler):
        _tsm(tsm),
        _device(device),
        _internalHandler(internalHandler),
        _externalHandler(externalHandler)
{
}


InternalConfirmedRequestHandler::~InternalConfirmedRequestHandler()
{
    delete _reqData;
    delete _service;
}

void InternalConfirmedRequestHandler::setRequester(BacnetAddress &address)
{
    _requester = address;
}

void InternalConfirmedRequestHandler::setRequestData(BacnetConfirmedRequestData *reqData)
{
    _reqData = reqData;
}

void InternalConfirmedRequestHandler::setService(BacnetService *service)
{
    _service = service;
}

bool InternalConfirmedRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    return _service->asynchActionFinished(asynchId, result, device, object);
}

bool InternalConfirmedRequestHandler::isFinished()
{
    return _service->isReady();
}

void InternalConfirmedRequestHandler::finalizeInstant(BacnetAddress &address, Bacnet::BacnetTSM2 *tsm,
                                                   BacnetConfirmedRequestData *requestPci, BacnetService *requestService)
{
    BacnetService *response(0);
    BacnetPciData *responsePci(0);
    if (requestService->hasError()) {
        response  = new BacnetErrorAck(requestService->error());
        responsePci = new BacnetErrorData(requestPci->invokedId(),
                                          requestPci->service());
    } else {
         response = requestService->takeResponse();
         if (0 == response) {//there is no response, thuss we send just simple ack.
            responsePci = new BacnetSimpleAckData(requestPci->invokedId(),
                                                  requestPci->service());
         } else {
             responsePci = new BacnetComplexAckData(requestPci->invokedId(), requestPci->service(),
                                                    0, 0, false, false);
         }
    }
    Q_CHECK_PTR(responsePci);

    Bacnet::AsynchronousTsmResponseAction *responseTsmAction =
            new Bacnet::AsynchronousTsmResponseAction(responsePci, response);

    tsm->sendAction(address, responseTsmAction);
    qDebug("Don't forget to delete erquestPci and requestService");
}

void InternalConfirmedRequestHandler::finalize(bool *deleteAfter)
{
    finalizeInstant(_requester, _tsm, _reqData, _service);
    Q_CHECK_PTR(deleteAfter);
    if (0 != deleteAfter)
        *deleteAfter = true;//I am not needed anymore
}
