#include "whohasservicehandler.h"

#include "internalobjectshandler.h"
#include "ihaveservicedata.h"
#include "bacnetobject.h"
#include "bacnettsm2.h"

using namespace Bacnet;

/**
  DON"T USE THIS CLASS ANYMORE. INSTEAD USE InternalWhoIsRequestHandler.
  */

WhoHasServiceHandler::WhoHasServiceHandler(InternalObjectsHandler *internalHndlr, BacnetTSM2 *tsm, BacnetDeviceObject *device):
        _internalHndlr(internalHndlr),
        _tsm(tsm),
        _device(device)
{
    Q_CHECK_PTR(internalHndlr);
    Q_CHECK_PTR(tsm);
    //if device is 0, this means we have broadcast.
}

qint32 WhoHasServiceHandler::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    return _data.fromRaw(serviceData, buffLength);
}

bool WhoHasServiceHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;//in case it is, tell it's done.
}

bool WhoHasServiceHandler::isFinished()
{
    return true;
}

void WhoHasServiceHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    if (deleteAfter)
        *deleteAfter = true;
}

QList<int> WhoHasServiceHandler::execute()
{
    QList<BacnetDeviceObject*> devs = _internalHndlr->devices();
    QList<BacnetDeviceObject*>::iterator devIt = devs.begin();
    QList<BacnetDeviceObject*>::iterator devListEnd = devs.end();

    BacnetAddress sender;
    BacnetAddress receiver;

    //! \todo If not all the responses fit in the buffer divide it in some chunks and get asynchIds
    quint32 objIdNum;
    IHaveServiceData iHaveData;
    quint32 minDevId = _data._rangeLowLimit;
    quint32 maxDevId = _data._rangeHighLimit;
    if (InvalidInstanceNumber == minDevId) {
        minDevId = 0;
        maxDevId = MaximumInstanceNumber;
    }

    QMap<quint32, BacnetObject*>::ConstIterator objIt;
    QMap<quint32, BacnetObject*>::ConstIterator objMapEnd;

    for (; devIt != devListEnd; ++devIt) {
        Jeszce tutaj niedokończone.
        objIdNum = (*devIt)->objectIdNum() & MaximumInstanceNumber;
        if ( (minDevId <= objIdNum) &&
             (objIdNum <= maxDevId) ) {
            //iterate over device's child objects
            objIt = (*devIt)->childObjects().begin();
            objMapEnd = (*devIt)->childObjects().end();
            for (; objIt != objMapEnd; ++objIt) {
                if (0 != _data._objidentifier) {
                    if ((*objIt)->objectIdNum() == (*_data._objidentifier)) {
                        iHaveData._devId = objIdNum;
                        iHaveData._objId = *_data._objidentifier;
                        iHaveData._objName = (*objIt)->objectName();
                        _tsm->sendUnconfirmed(sender, receiver, iHaveData, BacnetServices::I_Have);
                    }
                } else if (0 != _data._objName) {
                    if ((*objIt)->objectName() == (*_data._objName)) {
                        iHaveData._devId = objIdNum;
                        iHaveData._objId = (*objIt)->objIdNum();
                        iHaveData._objName = *_data._objName;
                        _tsm->sendUnconfirmed(sender, receiver, iHaveData, BacnetServices::I_Have);
                    }
                }
            }
        }
    }

    return QList<int>();
}
