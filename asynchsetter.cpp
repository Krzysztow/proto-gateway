#include "asynchsetter.h"

#include "property.h"
#include "bacnetpci.h"
#include "bacnetservice.h"
#include "servicefactory.h"
#include "bacnetcommon.h"

#include <QDebug>

#include "bacnetreadpropertyservice.h"
#include "bacnetreadpropertyack.h"
#include "helpercoder.h"
#include "bacnetwritepropertyservice.h"
#include "bacnetdeviceobject.h"
#include "asynchronousconfirmedhandler.h"
#include "bacnettsm.h"

void AsynchSetter::getBytes(quint8 *data, quint16 length)
{
    BacnetConfirmedRequestData *crData = new BacnetConfirmedRequestData();
    qint32 ret = crData->fromRaw(data, length);

    BacnetAddress address;//this address should come from the lower layer - this should indicate.

    //READ PROPERTY TEST
    //    Q_ASSERT(ret > 0);//send reject
//    ReadPropertyServiceHandler *service = new ReadPropertyServiceHandler();
//    ret = service->fromRaw(data + ret, length - ret);
//    Q_ASSERT(ret > 0);//send reject

//    WRITE PROPERTY TEST
    Q_ASSERT(ret > 0);//send reject
    BacnetService *service = ServiceFactory::createService(data+ret, length-ret, crData->service(), &ret);
    Q_ASSERT(ret > 0);//send reject

    //find device by address from network layer
    BacnetDeviceObject *device = _devices[0];//this should be the device address! NOT ZERO

    ret = service->execute(device);
    if (ret <= 0) {
        Q_ASSERT(service->isReady());
        InternalConfirmedRequestHandler::finalizeInstant(address, _tsm,
                                                      crData, service);
        delete service;//reclaim allocated memory
        delete crData;
        return;
    }
    //asynchronous action is to be executed.
    InternalConfirmedRequestHandler *handler = new InternalConfirmedRequestHandler(_tsm, device);
    handler->setRequestData(crData);//takes ownership over crData - don't delete.
    handler->setRequester(address);
    handler->setService(service);   //takse ownership over service - don't delete it.
    _asynchRequests.insert(ret, handler);
}


#include "helpercoder.h"
#include "analoginputobject.h"
//void AsynchSetter::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
void AsynchSetter::propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_ASSERT(_asynchRequests.contains(asynchId));

    Q_CHECK_PTR(device);
    Q_CHECK_PTR(object);

    InternalRequestHandler *serviceActHndlr = _asynchRequests.take(asynchId);
    if (!serviceActHndlr->asynchActionFinished(asynchId, result, object, device)) {
        return;
    }

    bool deleteHandler(true);
    serviceActHndlr->finalize(&deleteHandler);
    if (deleteHandler) {
        delete serviceActHndlr;
    }
}

int AsynchSetter::propertyRequested(BacnetDeviceObject *device, BacnetObject *object, BacnetProperty::Identifier propId)
{
    //find if we manage property. If not - return Property Unknown.

    //If it is, check if we we are subscribed by COV. If so, our value is accurate - return this one.

    //If we are not subscribed - create a request, send it and get AsynchId, and return.
}


bool AsynchSetter::addDevice(int address, BacnetDeviceObject *device)
{
    Q_ASSERT(!_devices.contains(address));
    if (_devices.contains(address))
        return false;

    _devices.insert(address, device);
    device->setHandler(this);
    return true;
}

AsynchSetter::AsynchSetter(Bacnet::BacnetTSM2 *tsm):
        _tsm(tsm)
{
    Q_CHECK_PTR(_tsm);
}


