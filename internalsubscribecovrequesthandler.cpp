#include "internalsubscribecovrequesthandler.h"

#include "bacnetcommon.h"
#include "iamservicedata.h"
#include "internalobjectshandler.h"
#include "bacnetdeviceobject.h"
#include "bacnettsm2.h"

using namespace Bacnet;

InternalSubscribeCOVRequestHandler::InternalSubscribeCOVRequestHandler(Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device,
                                                         InternalObjectsHandler *internalHandler, ExternalObjectsHandler *externalHandler):
InternalUnconfirmedRequestHandler(tsm, device, internalHandler, externalHandler),
_tsm(tsm),
_device(device),
_internalHandler(internalHandler),
_externalHandler(externalHandler)
{
}

InternalSubscribeCOVRequestHandler::~InternalSubscribeCOVRequestHandler()
{

}

bool InternalSubscribeCOVRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;//in case it is, tell it's done.
}

bool InternalSubscribeCOVRequestHandler::isFinished()
{
    return true;
}

void InternalSubscribeCOVRequestHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    if (deleteAfter)
        *deleteAfter = true;}

QList<int> InternalSubscribeCOVRequestHandler::execute()
{
    //! \todo Hange the way it calculates limits - there should be function for that.
    quint32 lowLimit = _data._rangeLowLimit | (BacnetObjectType::Device << 22);
    quint32 highLimit = _data._rangeHighLimit | (BacnetObjectType::Device << 22);

    QList<BacnetDeviceObject*> devs = _internalHandler->devices();
    QList<BacnetDeviceObject*>::iterator devIt = devs.begin();
    QList<BacnetDeviceObject*>::iterator devListEnd = devs.end();

    //! \todo If not all the responses fit in the buffer divide it in some chunks and get asynchIds
    quint32 objIdNum;
    ObjectIdStruct tmp = {BacnetObjectType::Undefined, 0};
    IAmServiceData iAmData(tmp, Bacnet::ApduMaxSize, SegmentedNOT, SNGVendorIdentifier);

    for (; devIt != devListEnd; ++devIt) {
        objIdNum = (*devIt)->objectIdNum();
        if ( (lowLimit <= objIdNum) &&
             (objIdNum <= highLimit) ) {
            iAmData._objId = (*devIt)->objectId();
            _tsm->sendUnconfirmed(_requester, _destination, iAmData, BacnetServices::I_Am);
        }
    }

    return QList<int>();
}

qint32 InternalSubscribeCOVRequestHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
