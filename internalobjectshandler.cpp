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
#include "internalunconfirmedrequesthandler.h"

void InternalObjectsHandler::getBytes(quint8 *data, quint16 length, BacnetAddress &srcAddress, BacnetAddress &destAddress)
{
    //looking for a destination device.
    BacnetDeviceObject *device(0);
    if (destAddress.isGlobalBroadcast() || destAddress.isGlobalBroadcast()) {
        device = 0;
    } else {
        InternalAddress destination = BacnetInternalAddressHelper::internalAddress(destAddress);
        if (BacnetInternalAddressHelper::InvalidInternalAddress == destination) {
            qDebug("InternalObjectsHandler::getBytes() : invalid address gotten.");
            return;
        }
        device = _devices[destination];
        //find device by address from network layer
        if (0 == device) {//device not found, drop it!
            qDebug("Device %d is not found!", destination);
            return;
        }
    }

    //handle accordingly to the request type.
    switch (BacnetPci::pduType(data))
    {
    case (BacnetPci::TypeConfirmedRequest):
        {
            /*upon reception:
                  - when no semgenation - do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
                  - when segmented - respond with BacnetSegmentAck PDU and when all gotten, do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
                 */
            BacnetConfirmedRequestData *crData = new BacnetConfirmedRequestData();
            qint32 ret = crData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            //! \todo What to send here? If we couldn't parse it we even have no data for reject (invoke id);
            if (ret <= 0) {
                delete crData;
                return;
            }

            InternalConfirmedRequestHandler *handler = ServiceFactory::createConfirmedHandler(crData, _tsm, device, this, _externalHandler);
            Q_CHECK_PTR(handler);
            if (0 == handler) {
                _tsm->sendReject(srcAddress, destAddress, BacnetReject::ReasonUnrecognizedService, crData->invokedId());
                delete crData;
                return;
            }

            ret = handler->fromRaw(data + ret, length - ret);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                //! \todo send reject - parsing should return the reject reason!
                _tsm->sendReject(srcAddress, destAddress, BacnetReject::ReasonMissingRequiredParameter, crData->invokedId());
                delete handler;
                return;
            }
            //! \todo Remove code duplication - with Unconfirmed request part.
            handler->setAddresses(srcAddress, destAddress);

            QList<int> returns = handler->execute();
            if (returns.isEmpty()) {//some error occured or is done. Both ways, we are ready to send respond back.
                Q_ASSERT(handler->isFinished());
                bool deleteAfter(true);
                handler->finalize(&deleteAfter);
                if (deleteAfter)
                    delete handler;
                return;
            }

            foreach (int asynchId, returns) {
                Q_ASSERT(!_asynchRequests.contains(asynchId));
                _asynchRequests.insert(asynchId, handler);
            }

            break;
        }
    case (BacnetPci::TypeUnconfirmedRequest): {
            /*upon reception: do what's needed and that's all
             */
            BacnetUnconfirmedRequestData *ucrData = new BacnetUnconfirmedRequestData();
            qint32 ret = ucrData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            //! \todo What to send here? If we couldn't parse it we even have no data for reject (invoke id);
            if (ret <= 0) {
                qDebug("Couldn't parse pci data, stops.");
                delete ucrData;
                return;
            }

            //create appropriate handler. \note It takes ownership over ucrData!
            InternalUnconfirmedRequestHandler *handler = ServiceFactory::createUnconfirmedHandler(ucrData, _tsm, device, this, _externalHandler);
            Q_CHECK_PTR(handler);
            if (0 == handler) {
                qDebug("InternalUnconfirmedRequestHandler not created, drop silently.");
                delete ucrData;
                return;
            }

            //set handler data
            ret = handler->fromRaw(data + ret, length - ret);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("InternalUnconfirmedRequestHandler parsed data incorrectnly (%d), drop silently!", ret);
                delete handler;
//                delete ucrData;//this is deleted with handler - it took ownership.
                return;
            }
            handler->setAddresses(srcAddress, destAddress);

            QList<int> returns = handler->execute();
            if (returns.isEmpty()) {//some error occured or is done. Both ways, we are ready to send respond back.
                Q_ASSERT(handler->isFinished());
                bool deleteAfter(true);
                handler->finalize(&deleteAfter);
                if (deleteAfter)
                    delete handler;
                return;
            }

            foreach (int asynchId, returns) {
                Q_ASSERT(!_asynchRequests.contains(asynchId));
                _asynchRequests.insert(asynchId, handler);
            }

            break;
        }
    case (BacnetPci::TypeSimpleAck):
        /*upon reception update state machine
             */
        break;
    case (BacnetPci::TypeComplexAck):
        /*upon reception update state machine
             */
        break;
    case (BacnetPci::TypeSemgmendAck):
        /*upon reception update state machine and send back another segment
             */
        break;
    case (BacnetPci::TypeError):
        /*BacnetConfirmedRequest seervice failed
             */
        break;
    case (BacnetPci::TypeReject):
        /*Protocol error occured
             */
        break;
    case (BacnetPci::TypeAbort):
        break;
    default: {
            Q_ASSERT(false);
        }
    }
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

bool InternalObjectsHandler::addDevice(InternalAddress address, BacnetDeviceObject *device)
{
    Q_ASSERT(!_devices.contains(address));
    if ( (BacnetInternalAddressHelper::InvalidInternalAddress == address) || _devices.contains(address))
        return false;

    _devices.insert(address, device);
    device->setHandler(this);
    return true;
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


