#include "bacnetapplicationlayer.h"

#include "bacnetpci.h"
#include "bacnetcoder.h"

#include "internalobjectshandler.h"
#include "externalobjectshandler.h"
#include "bacnettsm2.h"
#include "internalunconfirmedrequesthandler.h"
#include "internalconfirmedrequesthandler.h"
#include "servicefactory.h"

using namespace Bacnet;

BacnetApplicationLayerHandler::BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr):
        _networkHndlr(networkHndlr)
{
}

BacnetApplicationLayerHandler::~BacnetApplicationLayerHandler()
{
}

void BacnetApplicationLayerHandler::setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr)
{
    _networkHndlr = networkHndlr;
}

void BacnetApplicationLayerHandler::processConfirmedRequest(quint8 *dataPtr, quint16 dataLength)
{
    BacnetConfirmedRequestData serviceData;
    qint16 ret = serviceData.fromRaw(dataPtr, dataLength);
    if (ret >= 0) {
        qDebug("processConfirmedRequest() : incorrect PCI parsing!");
        Q_ASSERT(false);
        return;
    }

    //configure it dynamically
    switch (serviceData.service()) {
    case (BacnetServicesNS::ConfirmedCOVNotification): {
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    case (BacnetServicesNS::SubscribeCOV): {
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    case (BacnetServicesNS::ReadProperty): {
            //            BacnetReadProperty readPrpty;
            //            readPrpty.setFromRaw()
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    case (BacnetServicesNS::WriteProperty): {
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    default:
        qDebug("processConfirmedRequest() : Unrecognized service. Implement handler!");
        Q_ASSERT(false);
    }

    dataPtr += ret;
    dataLength -= ret;


}

void BacnetApplicationLayerHandler::indication(quint8 *data, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr)
{
    //looking for a destination device.
    Bacnet::BacnetDeviceObject *device(0);
    if (destAddr.isGlobalBroadcast() || destAddr.isGlobalBroadcast()){
        device = 0;
    } else {
        InternalAddress destination = BacnetInternalAddressHelper::internalAddress(destAddr);
        if (BacnetInternalAddressHelper::InvalidInternalAddress == destination) {
            qDebug("InternalObjectsHandler::getBytes() : invalid address gotten.");
            return;
        }
        //find device by address from network layer
        if (_externalHandler->isRegisteredAddress(destination)) {
            device = 0;
        } else {
            device = _internalHandler->virtualDevices()[destination];
            if (0 == device) {//device not found, drop request!
                qDebug("Device %d is not found!", destination);
                return;
            }
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
            if (0 == device) //confirmed request means we should have a device!
                return;

            BacnetConfirmedRequestData *crData = new BacnetConfirmedRequestData();
            qint32 ret = crData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            //! \todo What to send here? If we couldn't parse it we even have no data for reject (invoke id);
            if (ret <= 0) {
                delete crData;
                return;
            }

            InternalConfirmedRequestHandler *handler = ServiceFactory::createConfirmedHandler(crData, _tsm, device, _internalHandler, _externalHandler);
            Q_CHECK_PTR(handler);
            if (0 == handler) {
                _tsm->sendReject(srcAddr, destAddr, BacnetRejectNS::ReasonUnrecognizedService, crData->invokedId());
                delete crData;
                return;
            }

            ret = handler->fromRaw(data + ret, length - ret);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                //! \todo send reject - parsing should return the reject reason!
                _tsm->sendReject(srcAddr, destAddr, BacnetRejectNS::ReasonMissingRequiredParameter, crData->invokedId());
                delete handler;
                return;
            }
            //! \todo Remove code duplication - with Unconfirmed request part.
            handler->setAddresses(srcAddr, destAddr);

            bool readyToBeDeleted = handler->execute();
            if (readyToBeDeleted) {//some error occured or is done. Both ways, we are ready to send respond back.
                Q_ASSERT(handler->isFinished());
                delete handler;
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
            InternalUnconfirmedRequestHandler *handler = ServiceFactory::createUnconfirmedHandler(ucrData, _tsm, device, _internalHandler, _externalHandler);
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
            handler->setAddresses(srcAddr, destAddr);

            bool readyToBeDeleted = handler->execute();
            if (readyToBeDeleted) {//some error occured or is done. Both ways, we are ready to send respond back.
                Q_ASSERT(handler->isFinished());
                delete handler;
            }

            break;
        }
    case (BacnetPci::TypeSimpleAck):
        {
            BacnetSimpleAckData *saData = new BacnetSimpleAckData();
            qint32 ret = saData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("BacnetApplicationLayerHandler::indication() : wrong simple ack data (%d)", ret);
                delete saData;
                return;
            }
            //take ownership over the data!
            _tsm->receive(srcAddr, destAddr, saData);
            break;
        }
    case (BacnetPci::TypeComplexAck):
        {
            BacnetComplexAckData *cplxData = new BacnetComplexAckData();
            qint32 ret = cplxData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("BacnetApplicationLayerHandler::indication() : wrong complex ack data (%d)", ret);
                delete cplxData;
                return;
            }
            //take ownership over the data!
            _tsm->receive(srcAddr, destAddr, cplxData, data + ret, length - ret);
            break;
        }
    case (BacnetPci::TypeSemgmendAck):
        {
            /*upon reception update state machine and send back another segment
             */
            BacnetSegmentedAckData *segData = new BacnetSegmentedAckData();
            qint32 ret = segData ->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("BacnetApplicationLayerHandler::indication() : wrong complex ack data (%d)", ret);
                delete segData;
                return;
            }
            //take ownership over the data!
            _tsm->receive(srcAddr, destAddr, segData , data + ret, length - ret);
            break;
        }
    case (BacnetPci::TypeError):
        {
            /*BacnetConfirmedRequest seervice failed
             */
            BacnetErrorData *errData = new BacnetErrorData();
            qint32 ret = errData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("BacnetApplicationLayerHandler::indication() : wrong error data (%d)", ret);
                delete errData;
                return;
            }
            //take ownership over the data!
            _tsm->receive(srcAddr, destAddr, errData , data + ret, length - ret);
        }
        break;
    case (BacnetPci::TypeReject):
        {
            /*Protocol error occured
             */
            BacnetRejectData *rjctData = new BacnetRejectData();
            qint32 ret = rjctData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("BacnetApplicationLayerHandler::indication() : wrong reject data (%d)", ret);
                delete rjctData;
                return;
            }
            //take ownership over the data!
            _tsm->receive(srcAddr, destAddr, rjctData , data + ret, length - ret);
        }
        break;
    case (BacnetPci::TypeAbort):
        {
            BacnetAbortData *abrtData = new BacnetAbortData();
            qint32 ret = abrtData->fromRaw(data, length);
            Q_ASSERT(ret > 0);
            if (ret <= 0) {
                qDebug("BacnetApplicationLayerHandler::indication() : wrong abort data (%d)", ret);
                delete abrtData;
                return;
            }
            //take ownership over the data!
            _tsm->receive(srcAddr, destAddr, abrtData , data + ret, length - ret);
        }
        break;
    default: {
            Q_ASSERT(false);
        }
    }



    //    Q_ASSERT(length >= 1);//we need at least first byte for PDU type recognition
    //    if (length < 1) {
    //        //send error
    //        return;
    //    }

    //    qint16 ret(0);
    //    //dispatch to the device!!!


    //    switch (BacnetPci::pduType(actualBytePtr))
    //    {
    //    case (BacnetPci::TypeConfirmedRequest):
    //        {
    //            /*upon reception:
    //              - when no semgenation - do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
    //              - when segmented - respond with BacnetSegmentAck PDU and when all gotten, do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
    //             */
    //            processConfirmedRequest(actualBytePtr, length);
    //            break;
    //        }
    //    case (BacnetPci::TypeUnconfirmedRequest):
    //        /*upon reception: do what's needed and that's all
    //         */
    //        break;
    //    case (BacnetPci::TypeSimpleAck):
    //        /*upon reception update state machine
    //         */
    //        break;
    //    case (BacnetPci::TypeComplexAck):
    //        /*upon reception update state machine
    //         */
    //        break;
    //    case (BacnetPci::TypeSemgmendAck):
    //        /*upon reception update state machine and send back another segment
    //         */
    //        break;
    //    case (BacnetPci::TypeError):
    //        /*BacnetConfirmedRequest seervice failed
    //         */
    //        break;
    //    case (BacnetPci::TypeReject):
    //        /*Protocol error occured
    //         */
    //        break;
    //    case (BacnetPci::TypeAbort):
    //        break;
    //    default: {
    //            Q_ASSERT(false);
    //        }
    //    }
}

#define BAC_APP_TEST
#ifndef BAC_APP_TEST
//int main()
//{
//    return 0;
//}
#else
#include <QDebug>
#include <QtCore>

#include "propertyowner.h"
#include "asynchowner.h"
#include <QCoreApplication>
#include <QObject>

#include <sys/time.h>

#include "bacnetcommon.h"

#include "helpercoder.h"
#include "bacnettagparser.h"

#include "helpercoder.h"
#include "bacnetprimitivedata.h"
#include "bacnetpci.h"
#include "analoginputobject.h"
#include "bacnetdeviceobject.h"
#include "bacnettsm2.h"
#include "bacnetinternaladdresshelper.h"
#include "bacnetnetworklayer.h"
#include "cdm.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataModel::instance();

    BacnetNetworkLayerHandler *netHandler = new BacnetNetworkLayerHandler();
    BacnetApplicationLayerHandler *appHandler = new BacnetApplicationLayerHandler(netHandler);

    Bacnet::BacnetTSM2 *tsm = new Bacnet::BacnetTSM2();

    InternalObjectsHandler *intHandler = new InternalObjectsHandler(tsm);
    Bacnet::ExternalObjectsHandler *extHandler = new Bacnet::ExternalObjectsHandler(tsm);
    InternalAddress extObjHandlerAddress = 32;
    extHandler->addRegisteredAddress(extObjHandlerAddress);

    appHandler->_externalHandler = extHandler;
    appHandler->_internalHandler = intHandler;
    appHandler->_tsm = tsm;

    QVariant test;
    test.setValue((double)72.3);

    BacnetAddress srcAddr;

    BacnetAddress destAddr;
    quint32 destAddrRaw(0x00000001);
    BacnetInternalAddressHelper::macAddressFromRaw((quint8*)&destAddrRaw, &destAddr);

    AsynchOwner *proto2 = new AsynchOwner();
    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Double);
    subject->setValue(test);
    proto2->addProperty(subject);

    Bacnet::BacnetDeviceObject *device = new Bacnet::BacnetDeviceObject(1, destAddrRaw);
    device->setObjectName("BacnetTestDevice");
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);
    device->addInternalProperty(BacnetPropertyNS::PresentValue, obs);
    intHandler->addDevice(BacnetInternalAddressHelper::internalAddress(destAddr), device);

    PropertySubject *subject2 = DataModel::instance()->createProperty(2, QVariant::Double);
    subject2->setValue(test);
    proto2->addProperty(subject2);

    PropertyObserver *obs2 = DataModel::instance()->createPropertyObserver(2);
//    AnalogInputObject *aio = new AnalogInputObject(5, device);
//    aio->setObjectName("HW_Setpoint");
//    aio->addInternalProperty(BacnetProperty::PresentValue, obs2);

    quint32 addr(0x00000003);
    BacnetAddress bAddr;
    BacnetInternalAddressHelper::macAddressFromRaw((quint8*)&addr, &bAddr);
    Bacnet::BacnetDeviceObject *device1 = new Bacnet::BacnetDeviceObject(8, addr);
    intHandler->addDevice(BacnetInternalAddressHelper::internalAddress(bAddr), device1);
    device1->setObjectName("BestDeviceEver");

//    AnalogInputObject *aio1 = new AnalogInputObject(3, device1);
//    aio1->setObjectName("OATemp");

//    PropertySubject *extSubject = DataModel::instance()->createProperty(3, QVariant::Double);
//    extHandler->addMappedProperty(extSubject, BacnetObjectType::AnalogValue << 22 | 0x01,
//                                  BacnetProperty::PresentValue, Bacnet::ArrayIndexNotPresent,
//                                  0x00000001,
//                                  Bacnet::BacnetExternalObjects::Access_ReadRequest);

    PropertyObserver *extObserver = DataModel::instance()->createPropertyObserver(3);
    proto2->addProperty(extObserver);

    //READ PROPERTY ENCODED
    quint8 readPropertyService[] = {
        0x00,
        0x00,
        0x01,
        0x0C,
        0x0C,
        0x00, 0x00, 0x00, 0x05,
        0x19,
        0x55
    };
    appHandler->indication(readPropertyService, sizeof(readPropertyService), srcAddr, destAddr);

    //    //WRITE PROEPRTY ENCODED
    //    quint8 wpService[] = {
    //        0x00,
    //        0x04,
    //        0x59,
    //        0x0F,

    //        0x0c,
    //        0x00, 0x00/\*0x80*\/, 0x00, 0x01,
    //        0x19,
    //        0x55,
    //        0x3e,
    //        0x44,
    //        0x43, 0x34, 0x00, 0x00,
    //        0x3f
    //    };
    //    bHndlr->getBytes(wpService, sizeof(wpService), srcAddr, destAddr);

    //    //WHO IS
    //    quint8 wiService[] = {
    //        0x10,
    //        0x08,
    //        0x09, 0x03,
    //        0x19, 0x03
    //    };
    //    bHndlr->getBytes(wiService, sizeof(wiService), srcAddr, destAddr);

    //    //WHO HAS - object name is known
    //    quint8 whoHasService[] = {
    //        0x10,
    //        0x07,
    //        0x3d,
    //        0x07,
    //        0x00,
    //        0x4F, 0x41, 0x54, 0x65, 0x6D, 0x70
    //    };

    //    BacnetAddress broadAddr;
    //    broadAddr.setGlobalBroadcast();
    //    bHndlr->getBytes(whoHasService, sizeof(whoHasService), srcAddr, broadAddr);

    //    //WHO HAS - object id is known
    //    quint8 whoHasService2[] = {
    //        0x10,
    //        0x07,
    //        0x2c,
    //        0x00, 0x00, 0x00, 0x03
    //    };
    //    bHndlr->getBytes(whoHasService2, sizeof(whoHasService2), srcAddr, broadAddr);



    return a.exec();
}

#endif
