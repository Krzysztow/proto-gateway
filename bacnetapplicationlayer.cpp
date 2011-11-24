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

BacnetApplicationLayerHandler::BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr, QObject *parent):
    QObject(parent),
    _networkHndlr(networkHndlr),
    _internalHandler(new InternalObjectsHandler(this)),
    _externalHandler(new ExternalObjectsHandler(this)),
    _tsm(new Bacnet::BacnetTSM2(this))
{
}

BacnetApplicationLayerHandler::~BacnetApplicationLayerHandler()
{
}

QList<Bacnet::BacnetDeviceObject*> BacnetApplicationLayerHandler::devices()
{
    Q_CHECK_PTR(_internalHandler);
    return _internalHandler->devices();
    //return _internalHandler->devices();
}

void BacnetApplicationLayerHandler::setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr)
{
    _networkHndlr = networkHndlr;
}

//! \note crData is to be taken ownership over. Otherwise memory leakage will occure.
void BacnetApplicationLayerHandler::processConfirmedRequest(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, BacnetConfirmedRequestData *crData)
{
    InternalAddress destination = BacnetInternalAddressHelper::internalAddress(localDestination);
    BacnetDeviceObject *device = _internalHandler->virtualDevices().value(destination);

    InternalConfirmedRequestHandler *handler = ServiceFactory::createConfirmedHandler(remoteSource, localDestination, crData, _tsm, device, this);
    Q_CHECK_PTR(handler);
    if (0 == handler) {
        _tsm->sendReject(remoteSource, localDestination, BacnetRejectNS::ReasonUnrecognizedService, crData->invokedId());
        delete crData;
        return;
    }

    qint32 ret = handler->fromRaw(dataPtr, dataLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        //! \todo send reject - parsing should return the reject reason!
        _tsm->sendReject(remoteSource, localDestination, BacnetRejectNS::ReasonMissingRequiredParameter, crData->invokedId());
        delete handler;
        return;
    }

    bool readyToBeDeleted = handler->execute();
    //! \note If readyToBeDeleted is false, that means the handler handed itself to either Internal or External objects handler.
    if (readyToBeDeleted) {//some error occured or is done. Both ways, we are ready to send respond back.
        Q_ASSERT(handler->isFinished());
        delete handler;
    }
}

void BacnetApplicationLayerHandler::processUnconfirmedRequest(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, BacnetUnconfirmedRequestData &ucrData)
{
    InternalAddress destination = BacnetInternalAddressHelper::internalAddress(localDestination);
    BacnetDeviceObject *device = _internalHandler->virtualDevices().value(destination);

    //create appropriate handler. \note It takes ownership over ucrData!
    InternalUnconfirmedRequestHandler *handler = ServiceFactory::createUnconfirmedHandler(remoteSource, localDestination, ucrData, _tsm, device, this);
    Q_CHECK_PTR(handler);
    if (0 == handler) {
        qDebug("InternalUnconfirmedRequestHandler not created, drop silently, it's unconfirmed service.");
        return;
    }

    //set handler data
    qint32 ret = handler->fromRaw(dataPtr, dataLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        qDebug("InternalUnconfirmedRequestHandler parsed data incorrectnly (%d), drop silently!", ret);
        delete handler;
        return;
    }

    bool readyToBeDeleted = handler->execute();
    if (readyToBeDeleted) {//some error occured or is done. Both ways, we are ready to send respond back.
        Q_ASSERT(handler->isFinished());
        delete handler;
    }
}

void BacnetApplicationLayerHandler::processAck(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct)
{

}

void BacnetApplicationLayerHandler::processError(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct)
{

}

void BacnetApplicationLayerHandler::processReject(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct)
{
}

void BacnetApplicationLayerHandler::processAbort(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct)
{
}

void Bacnet::BacnetApplicationLayerHandler::processTimeout(ExternalConfirmedServiceHandler *serviceAct)
{
}


void BacnetApplicationLayerHandler::sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    Q_ASSERT(false);//need to implement it!
}

void BacnetApplicationLayerHandler::sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    _tsm->sendUnconfirmed(destination, source, data, serviceChoice);
}

bool BacnetApplicationLayerHandler::send(const Bacnet::ObjectIdStruct &destinedObject, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms)
{
    Q_ASSERT(false);
}

bool BacnetApplicationLayerHandler::send(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms)
{
    return _tsm->send(destination, sourceAddress, service, serviceToSend);
}

void BacnetApplicationLayerHandler::indication(quint8 *data, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr)
{
    //is it really intended for us?
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
            device = _internalHandler->virtualDevices().value(destination);
            if (0 == device) {//device not found, drop request!
                qDebug("Device %d is not found!", destination);
                return;
            }
        }
    }

    //tell Transaction State Machine to take care of the packet. If the packet is meaningful to the AppLayer, it will be returned (its part) by TSM.
    _tsm->receive(srcAddr, destAddr, data, length);
}


ExternalObjectsHandler *BacnetApplicationLayerHandler::externalHandler()
{
    return _externalHandler;
}

InternalObjectsHandler *BacnetApplicationLayerHandler::internalHandler()
{
    return _internalHandler;
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
#include "bacnetproperty.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataModel::instance();

    BacnetNetworkLayerHandler *netHandler = new BacnetNetworkLayerHandler();
    BacnetApplicationLayerHandler *appHandler = new BacnetApplicationLayerHandler(netHandler);

    InternalObjectsHandler *intHandler = appHandler->internalHandler();
    Bacnet::ExternalObjectsHandler *extHandler = appHandler->externalHandler();

    InternalAddress extObjHandlerAddress = 32;
    extHandler->addRegisteredAddress(extObjHandlerAddress);


    QVariant test;
    test.setValue((double)72.3);

    BacnetAddress srcAddr;
    quint32 destAddrInt(0x00000001);
    BacnetAddress destAddr = BacnetInternalAddressHelper::toBacnetAddress(destAddrInt);

    AsynchOwner *proto2 = new AsynchOwner();
    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Double);
    subject->setValue(test);
    proto2->addProperty(subject);

    Bacnet::BacnetDeviceObject *device = new Bacnet::BacnetDeviceObject(1, destAddr);
    HelperCoder::printArray(destAddr.macPtr(), destAddr.macAddrLength(), "Device 1 address");
    device->setObjectName("BacnetTestDevice");
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);
    Bacnet::ProxyInternalProperty propProxy(obs, AppTags::Real, QVariant::Double, device);
    device->addProperty(BacnetPropertyNS::PresentValue, &propProxy);
    intHandler->addDevice(device->address(), device);

    PropertySubject *subject2 = DataModel::instance()->createProperty(2, QVariant::Double);
    subject2->setValue(test);
    proto2->addProperty(subject2);

    PropertyObserver *obs2 = DataModel::instance()->createPropertyObserver(2);
    BacnetObject *aio = new BacnetObject(BacnetObjectTypeNS::AnalogInput, 5, device);
    //    AnalogInputObject *aio = new AnalogInputObject(5, device);
    aio->setObjectName("OATemp");
    Bacnet::ProxyInternalProperty propProxy2(obs2, AppTags::Real, QVariant::Double, aio);
    aio->addProperty(BacnetPropertyNS::PresentValue, &propProxy2);

    quint32 addr(0x00000003);
    BacnetAddress bAddr;
    BacnetInternalAddressHelper::macAddressFromRaw((quint8*)&addr, &bAddr);
    Bacnet::BacnetDeviceObject *device1 = new Bacnet::BacnetDeviceObject(8, bAddr);
    intHandler->addDevice(device1->address(), device1);
    device1->setObjectName("BestDeviceEver");

    //    BacnetObject *aio1 = new BacnetObject(BacnetObjectTypeNS::AnalogInput, 3, device1);
    ////    AnalogInputObject *aio1 = new AnalogInputObject(3, device1);
    //    aio1->setObjectName("OATemp");

    //    PropertySubject *extSubject = DataModel::instance()->createProperty(3, QVariant::Double);
    //    extHandler->addMappedProperty(extSubject, BacnetObjectType::AnalogValue << 22 | 0x01,
    //                                  BacnetProperty::PresentValue, Bacnet::ArrayIndexNotPresent,
    //                                  0x00000001,
    //                                  Bacnet::BacnetExternalObjects::Access_ReadRequest);

    //    PropertyObserver *extObserver = DataModel::instance()->createPropertyObserver(3);
    //    proto2->addProperty(extObserver);

    //READ PROPERTY ENCODED
    //    quint8 readPropertyService[] = {
    //        0x00,
    //        0x00,
    //        0x01,
    //        0x0C,
    //        0x0C,
    //        0x00, 0x00, 0x00, 0x05,
    //        0x19,
    //        0x55
    //    };

    //    HelperCoder::printArray(destAddr.macPtr(), destAddr.macAddrLength(), "Addressed device:");
    //    appHandler->indication(readPropertyService, sizeof(readPropertyService), srcAddr, destAddr);

    //        //WRITE PROEPRTY ENCODED
    //        quint8 wpService[] = {
    //            0x00,
    //            0x04,
    //            0x59,
    //            0x0F,

    //            0x0c,
    //            0x00, 0x00/*0x80*/, 0x00, /*0x01*/0x05, //analog input instance number 5
    //            0x19,
    //            0x55,
    //            0x3e,
    //            0x44,
    //            0x43, 0x34, 0x00, 0x00,
    //            0x3f
    //        };
    //        appHandler->indication(wpService, sizeof(wpService), srcAddr, destAddr);

    //        //WHO IS//device instance 03
    //        quint8 wiService[] = {
    //            0x10,
    //            0x08,
    //            0x09, 0x03,//find device
    //            0x19, 0x03
    //        };
    //        appHandler->indication(wiService, sizeof(wiService), srcAddr, destAddr);

    //    //WHO IS//device instance 03
    //    quint8 wiService[] = {
    //        0x10,
    //        0x08,
    //        0x09, 0x01,//find device
    //        0x19, 0x01
    //    };
    //    appHandler->indication(wiService, sizeof(wiService), srcAddr, destAddr);

    //    //WHO HAS - object name is known
    //    quint8 whoHasService[] = {
    //        0x10,
    //        0x07,
    //        0x3d,
    //        0x07,
    //        0x00,
    //        0x4F, 0x41, 0x54, 0x65, 0x6D, 0x70
    //    };
    //    appHandler->indication(whoHasService, sizeof(whoHasService), srcAddr, destAddr);

    //        BacnetAddress broadAddr;
    //        broadAddr.setGlobalBroadcast();
    ////        bHndlr->getBytes(whoHasService, sizeof(whoHasService), srcAddr, broadAddr);

    //        //WHO HAS - object id is known
    //        quint8 whoHasService2[] = {
    //            0x10,
    //            0x07,
    //            0x2c,
    //            0x00, 0x00, 0x00, 0x05
    //        };
    //        appHandler->indication(whoHasService2, sizeof(whoHasService2), srcAddr, broadAddr);



    return a.exec();
}

#endif
