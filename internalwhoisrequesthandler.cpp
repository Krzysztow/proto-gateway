#include "internalwhoisrequesthandler.h"

#include "bacnetcommon.h"
#include "iamservicedata.h"
#include "internalobjectshandler.h"
#include "bacnetdeviceobject.h"
#include "bacnettsm2.h"

using namespace Bacnet;

InternalWhoIsRequestHandler::InternalWhoIsRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                         InternalObjectsHandler *internalHandler, ExternalObjectsHandler *externalHandler):
InternalUnconfirmedRequestHandler(),
_tsm(tsm),
_device(device),
_internalHandler(internalHandler),
_externalHandler(externalHandler)
{
}

InternalWhoIsRequestHandler::~InternalWhoIsRequestHandler()
{

}

bool InternalWhoIsRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;//in case it is, tell it's done.
}

bool InternalWhoIsRequestHandler::isFinished()
{
    return true;
}

void InternalWhoIsRequestHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    if (deleteAfter)
        *deleteAfter = true;}

bool InternalWhoIsRequestHandler::execute()
{
    //! \todo Hange the way it calculates limits - there should be function for that.
    quint32 lowLimit = _data._rangeLowLimit | (BacnetObjectTypeNS::Device << 22);
    quint32 highLimit = _data._rangeHighLimit | (BacnetObjectTypeNS::Device << 22);

    QList<BacnetDeviceObject*> devs = _internalHandler->devices();
    QList<BacnetDeviceObject*>::iterator devIt = devs.begin();
    QList<BacnetDeviceObject*>::iterator devListEnd = devs.end();

    //! \todo If not all the responses fit in the buffer divide it in some chunks and get asynchIds
    quint32 objIdNum;
    ObjectIdentifier tmp(BacnetObjectTypeNS::Undefined, 0);
    IAmServiceData iAmData(tmp, Bacnet::ApduMaxSize, SegmentedNOT, SNGVendorIdentifier);

    for (; devIt != devListEnd; ++devIt) {
        objIdNum = (*devIt)->objectIdNum();
        if ( (lowLimit <= objIdNum) &&
             (objIdNum <= highLimit) ) {
            iAmData._objId = (*devIt)->objectIdNum();
            _tsm->sendUnconfirmed(_requester, _destination, iAmData, BacnetServicesNS::I_Am);
        }
    }

    //all is sent - am ready to be deleted!
    return true;
}

qint32 InternalWhoIsRequestHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
