#include "internalobjectshandler.h"

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
#include "bacnetservice.h"
#include "bacnettsm2.h"

void InternalObjectsHandler::getBytes(quint8 *data, quint16 length, BacnetAddress &srcAddress, BacnetAddress &destAddress)
{

}

#include "helpercoder.h"
#include "analoginputobject.h"
//void AsynchSetter::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
void InternalObjectsHandler::propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
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

void InternalObjectsHandler::addAsynchronousHandler(QList<int> asynchIds, InternalRequestHandler *handler)
{
    foreach (int asynchId, asynchIds) {
        Q_ASSERT(!_asynchRequests.contains(asynchId));
        _asynchRequests.insert(asynchId, handler);
    }
}

bool InternalObjectsHandler::addDevice(InternalAddress address, BacnetDeviceObject *device)
{
    Q_ASSERT(!_devices.contains(address));
    if ( (BacnetInternalAddressHelper::InvalidInternalAddress == address) || _devices.contains(address))
        return false;

    _devices.insert(address, device);
    device->setHandler(this);
    return true;
}

QMap<quint32, BacnetDeviceObject*> &InternalObjectsHandler::virtualDevices()
{
    return _devices;
}

QList<BacnetDeviceObject*> InternalObjectsHandler::devices()
{
    return _devices.values();
}

InternalObjectsHandler::InternalObjectsHandler(Bacnet::BacnetTSM2 *tsm):
        _tsm(tsm)
{
    Q_CHECK_PTR(_tsm);
}


