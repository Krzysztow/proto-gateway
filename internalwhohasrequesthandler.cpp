#include "internalwhohasrequesthandler.h"

#include "bacnetcommon.h"
#include "internalobjectshandler.h"
#include "ihaveservicedata.h"
#include "bacnetdeviceobject.h"
#include "bacnettsm2.h"

using namespace Bacnet;

InternalWhoHasRequestHandler::InternalWhoHasRequestHandler(BacnetUnconfirmedRequestData *reqData,
                                                           BacnetAddress &requester,
                                                           Bacnet::BacnetTSM2 *tsm, BacnetDeviceObject *device, InternalObjectsHandler *internalHandler):
    InternalUnconfirmedRequestHandler(reqData),
    _tsm(tsm),
    _device(device),
    _requester(requester),
    _internalHandler(internalHandler)
{
}

InternalWhoHasRequestHandler::~InternalWhoHasRequestHandler()
{
}

bool InternalWhoHasRequestHandler::asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(result);
    Q_UNUSED(object);
    Q_UNUSED(device);
    Q_ASSERT(false);//shouldn't be invoked
    return true;//in case it is, tell it's done.
}

bool InternalWhoHasRequestHandler::isFinished()
{
    return true;
}

void InternalWhoHasRequestHandler::finalize(bool *deleteAfter)
{
    Q_CHECK_PTR(deleteAfter);
    if (deleteAfter)
        *deleteAfter = true;}

bool InternalWhoHasRequestHandler::execute()
{
    //    QList<BacnetDeviceObject*> devs = _internalHandler->devices();
    //    QList<BacnetDeviceObject*>::iterator devIt = devs.begin();
    //    QList<BacnetDeviceObject*>::iterator devListEnd = devs.end();

    //    //! \todo If not all the responses fit in the buffer divide it in some chunks and get asynchIds
    //    quint32 objIdNum;
    //    ObjectIdStruct tmp = {BacnetObjectType::Undefined, 0};
    //    IAmServiceData iAmData(tmp, Bacnet::ApduMaxSize, SegmentedNOT, SNGVendorIdentifier);

    //    QMap<quint32, BacnetObject*>::ConstIterator objIt;
    //    QMap<quint32, BacnetObject*>::ConstIterator objMapEnd;

    //    for (; devIt != devListEnd; ++devIt) {
    //        objIdNum = (*devIt)->objectIdNum();
    //        if ( (_data._rangeLowLimit <= objIdNum) &&
    //             (objIdNum <= _data._rangeHighLimit) ) {
    //            iAmData._objId = (*devIt)->objectId();
    //            _tsm->sendUnconfirmed(_requester, _destination, iAmData, BacnetServices::I_Am);
    //        }
    //        //iterate over device's child objects
    //        objIt = (*devIt)->childObjects().begin();
    //        objMapEnd = (*devIt)->childObjects().end();
    //        for (; objIt != objMapEnd; ++objIt) {
    //            //! \todo Optimization may be aquired here by taking objectIdNum only once.
    //            if ( (_data._rangeLowLimit <= (*objIt)->objectIdNum()) &&
    //                 ((*objIt)->objectIdNum() <= _data._rangeHighLimit) ) {
    //                iAmData._objId = (*objIt)->objectId();
    //                _tsm->sendUnconfirmed(_requester, _destination, iAmData, BacnetServices::I_Am);
    //            }
    //        }
    //    }

    //    return QList<int>();

    QList<BacnetDeviceObject*> devs = _internalHandler->devices();
    QList<BacnetDeviceObject*>::iterator devIt = devs.begin();
    QList<BacnetDeviceObject*>::iterator devListEnd = devs.end();

    //! \todo If not all the responses fit in the buffer divide it in some chunks and get asynchIds
    quint32 devInstanceNum;
    IHaveServiceData iHaveData;
    quint32 minDevId = _data._rangeLowLimit;
    quint32 maxDevId = _data._rangeHighLimit;
#warning "Change object Id struct to the quint32"
    quint32 searchedObjInstance = InvalidInstanceNumber;
    if (0 != _data._objidentifier)
        searchedObjInstance = objIdToNum(*_data._objidentifier);
    if (InvalidInstanceNumber == minDevId) {
        minDevId = 0;
        maxDevId = MaximumInstanceNumber;
    }

    QMap<quint32, BacnetObject*>::ConstIterator objIt;
    QMap<quint32, BacnetObject*>::ConstIterator objMapEnd;

    for (; devIt != devListEnd; ++devIt) {
        devInstanceNum = (*devIt)->objectIdNum() & ObjectInstanceMask;
        if ( (minDevId <= devInstanceNum) &&
             (devInstanceNum <= maxDevId) ) {
            //iterate over device's child objects
            objIt = (*devIt)->childObjects().begin();
            objMapEnd = (*devIt)->childObjects().end();
            for (; objIt != objMapEnd; ++objIt) {
                if ( ( (0 != _data._objidentifier) && ((*objIt)->objectIdNum() == searchedObjInstance) ) ||
                     ( (0 != _data._objName) && ((*objIt)->objectName() == (*_data._objName)) ) ) {
                    //manipulate object data.
                    iHaveData._devId.setObjectIdNum(devInstanceNum | BacnetObjectTypeNS::Device << 22);
                    iHaveData._objId = (*objIt)->objectId();
                    iHaveData._objName = (*objIt)->objectName();
                    _tsm->sendUnconfirmed(_requester, _destination, iHaveData, BacnetServicesNS::I_Have);
                    //we can stop here, since no two objects internetowrk-wide may have the same instance numbers or names.
                    return true;
                }
            }
        }
    }

    //no asynchronous actions - return empty list.
    return true;
}

qint32 InternalWhoHasRequestHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    return _data.fromRaw(servicePtr, length);
}
