#include "whoisservicehandler.h"

#include "bacnetcommon.h"
#include "bacnettsm2.h"
#include "internalobjectshandler.h"
#include "bacnetdeviceobject.h"
#include "bacnetdefaultobject.h"
#include "iamservicedata.h"

#include <QList>

using namespace Bacnet;

/**
  DON"T USE THIS CLASS ANYMORE. INSTEAD USE InternalWhoIsRequestHandler.
  */

WhoIsServiceHandler::WhoIsServiceHandler(InternalObjectsHandler *internalHndlr, BacnetTSM2 *tsm, BacnetDeviceObject *device):
        _internalHndlr(internalHndlr),
        _tsm(tsm),
        _device(device)
{
    Q_CHECK_PTR(internalHndlr);
    Q_CHECK_PTR(tsm);
    //if device is 0, this means we have broadcast.
}

qint32 WhoIsServiceHandler::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    return _data.fromRaw(serviceData, buffLength);
}

bool WhoIsServiceHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;//in case it is, tell it's done.
}

bool WhoIsServiceHandler::isFinished()
{
    return true;
}

void WhoIsServiceHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    if (deleteAfter)
        *deleteAfter = true;
}

QList<int> WhoIsServiceHandler::execute()
{
    QList<BacnetDeviceObject*> devs = _internalHndlr->devices();
    QList<BacnetDeviceObject*>::iterator devIt = devs.begin();
    QList<BacnetDeviceObject*>::iterator devListEnd = devs.end();

    BacnetAddress sender;
    BacnetAddress receiver;

    //! \todo If not all the responses fit in the buffer divide it in some chunks and get asynchIds
    quint32 objIdNum;
    ObjectIdStruct tmp = {BacnetObjectType::Undefined, 0};
    IAmServiceData iAmData(tmp, Bacnet::ApduMaxSize, SegmentedNOT, SNGVendorIdentifier);

    QMap<quint32, BacnetObject*>::ConstIterator objIt;
    QMap<quint32, BacnetObject*>::ConstIterator objMapEnd;

    for (; devIt != devListEnd; ++devIt) {
        objIdNum = (*devIt)->objectIdNum();
        if ( (_data._rangeLowLimit <= objIdNum) &&
             (objIdNum <= _data._rangeHighLimit) ) {
            iAmData._objId = (*devIt)->objectId();
            _tsm->sendUnconfirmed(receiver, sender, iAmData, BacnetServices::I_Am);
        }
        //iterate over device's child objects
        objIt = (*devIt)->childObjects().begin();
        objMapEnd = (*devIt)->childObjects().end();
        for (; objIt != objMapEnd; ++objIt) {
            //! \todo Optimization may be aquired here by taking objectIdNum only once.
            if ( (_data._rangeLowLimit <= (*objIt)->objectIdNum()) &&
                 ((*objIt)->objectIdNum() <= _data._rangeHighLimit) ) {
                iAmData._objId = (*objIt)->objectId();
                _tsm->sendUnconfirmed(receiver, sender, iAmData, BacnetServices::I_Am);
            }
        }
    }

    return QList<int>();
}
