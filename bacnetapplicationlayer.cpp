#include "bacnetapplicationlayer.h"

#include "bacnetpci.h"
#include "bacnetcoder.h"

#include "internalobjectshandler.h"
#include "externalobjectshandler.h"
#include "internalunconfirmedrequesthandler.h"
#include "internalconfirmedrequesthandler.h"
#include "servicefactory.h"
#include "whoisservicedata.h"
#include "whohasservicedata.h"
#include "discoverywrapper.h"
#include "externalconfirmedservicehandler.h"
#include "error.h"

using namespace Bacnet;

BacnetApplicationLayerHandler::BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr, QObject *parent):
    QObject(parent),
    _networkHndlr(networkHndlr),
    _internalHandler(new InternalObjectsHandler(this)),
    _externalHandler(new ExternalObjectsHandler(this)),
    _tsm(new Bacnet::BacnetTSM2(this)),
    _devicesRoutingTable(DefaultDynamicElementsSize),
    _objectDeviceMapper(DefaultMapperElementsSize)
{
    _timer.start(TimerInterval_ms, this);
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

    InternalConfirmedRequestHandler *handler = ServiceFactory::createConfirmedHandler(remoteSource, localDestination, crData, device, this);
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
    qDebug("--- Address length is %d", remoteSource.macAddrLength());
    InternalUnconfirmedRequestHandler *handler = ServiceFactory::createUnconfirmedHandler(remoteSource, localDestination, ucrData, device, this);
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

void BacnetApplicationLayerHandler::processAbort(BacnetAddress &remoteSource, BacnetAddress &localDestination, ExternalConfirmedServiceHandler *serviceAct)
{
    int idx = _awaitingConfirmedServices.indexOf(serviceAct);
    Q_ASSERT(idx >= 0);

    if (idx < 0) {
        qDebug("%s : gotten abort to an unexpected service!", __PRETTY_FUNCTION__);
        return;
    }

    ExternalConfirmedServiceHandler::ActionToExecute action(ExternalConfirmedServiceHandler::DoNothing);
    ExternalConfirmedServiceHandler *handler = serviceAct;
    if (0 != handler)
        action = handler->handleAbort();
    cleanUpService(remoteSource, localDestination, action, idx);
}

void BacnetApplicationLayerHandler::processReject(BacnetAddress &remoteSource, BacnetAddress &localDestination, BacnetRejectNS::RejectReason reason, ExternalConfirmedServiceHandler *serviceAct)
{
    int idx = _awaitingConfirmedServices.indexOf(serviceAct);
    Q_ASSERT(idx >= 0);

    if (idx < 0) {
        qDebug("%s : gotten reject to an unexpected service!", __PRETTY_FUNCTION__);
        return;
    }

    ExternalConfirmedServiceHandler::ActionToExecute action(ExternalConfirmedServiceHandler::DoNothing);
    ExternalConfirmedServiceHandler *handler = serviceAct;
    if (0 != handler)
        action = handler->handleReject(reason);
    cleanUpService(remoteSource, localDestination, action, idx);
}

void BacnetApplicationLayerHandler::processError(BacnetAddress &remoteSource, BacnetAddress &localDestination, Error &error, ExternalConfirmedServiceHandler *serviceAct)
{
    int idx = _awaitingConfirmedServices.indexOf(serviceAct);
    Q_ASSERT(idx >= 0);

    if (idx < 0) {
        qDebug("%s : gotten error to an unexpected service!", __PRETTY_FUNCTION__);
        return;
    }

    ExternalConfirmedServiceHandler::ActionToExecute action(ExternalConfirmedServiceHandler::DoNothing);
    ExternalConfirmedServiceHandler *handler = serviceAct;
    if (0 != handler)
        action = handler->handleError(error);
    cleanUpService(remoteSource, localDestination, action, idx);
}

void BacnetApplicationLayerHandler::cleanUpService(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 action, int idx)
{
    Q_ASSERT(idx >= 0);
    Q_ASSERT(idx < _awaitingConfirmedServices.count());
    //delete handlers if necessary.
    if (ExternalConfirmedServiceHandler::DeleteServiceHandler == action) {
        delete _awaitingConfirmedServices[idx];
    } else if (ExternalConfirmedServiceHandler::DoNothing == action) {
        ;
    } else if (ExternalConfirmedServiceHandler::ResendService == action) {
        send(remoteSource, localDestination, _awaitingConfirmedServices[idx]);
    }

    //remove ACT from the hash
    _awaitingConfirmedServices.removeAt(idx);
}


void BacnetApplicationLayerHandler::processAck(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct)
{
    Q_UNUSED(remoteSource);
    Q_UNUSED(localDestination);
    Q_CHECK_PTR(serviceAct);
    Q_CHECK_PTR(dataPtr);
    int idx = _awaitingConfirmedServices.indexOf(serviceAct);
    Q_ASSERT(idx >= 0);
    if (idx >= 0) {
        ExternalConfirmedServiceHandler::ActionToExecute action(ExternalConfirmedServiceHandler::DoNothing);
        ExternalConfirmedServiceHandler *handler = serviceAct;
        if (0 != handler)
            action = handler->handleAck(dataPtr, dataLength);
        cleanUpService(remoteSource, localDestination, action, idx);
    } else {
        qDebug("%s : service not in a list, but gotten! ERROR!", __PRETTY_FUNCTION__);
    }
}

void Bacnet::BacnetApplicationLayerHandler::processTimeout(BacnetAddress &remoteDestination, BacnetAddress &localSource, ExternalConfirmedServiceHandler *serviceAct)
{
    Q_CHECK_PTR(serviceAct);
    int idx = _awaitingConfirmedServices.indexOf(serviceAct);
    Q_ASSERT(idx >= 0);
    if ( idx >= 0 ) {
        ExternalConfirmedServiceHandler *handler = serviceAct;
        if (0 != handler) {
            //delete handlers if necessary.
            ExternalConfirmedServiceHandler::ActionToExecute action = handler->handleTimeout();
            if (ExternalConfirmedServiceHandler::DeleteServiceHandler == action) {
                delete handler;
            } else if (ExternalConfirmedServiceHandler::DoNothing == action) {
                ;
            } else if (ExternalConfirmedServiceHandler::ResendService == action) {
                send(remoteDestination, localSource, handler);
            }
        } else {
            qDebug("%s : service not in a list, but gotten! ERROR!", __PRETTY_FUNCTION__);
        }
        //remove ACT from the hash
        _awaitingConfirmedServices.removeAt(idx);
    } else {
        qDebug("%s : Timeout of the confirmed request, which was not in the list!", __PRETTY_FUNCTION__);
    }
}

bool BacnetApplicationLayerHandler::sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    bool found(true);
    quint32 objIdNum = objIdToNum(destinedObject);
    if (BacnetObjectTypeNS::Device !=  destinedObject.objectType) { //this is not a device, we have to find it's device object
        objIdNum = _objectDeviceMapper.findEntry(objIdNum, &found);//substitute locally objectIdNum with it's device number
    }

    if (found) { //Here, if foud is true, then objIdNum is the device instance. If so, try to find device address
        const mappingEntry &re = _devicesRoutingTable.findEntry(objIdNum, &found);//note that here objIdNum is id of the device
        if (found) {
            sendUnconfirmed(re.address, source, data, serviceChoice);
        }
    }

    return found;
}

//! \note Takes ownership over the data element!
bool BacnetApplicationLayerHandler::sendUnconfirmedWithDiscovery(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData *data, quint8 serviceChoice)
{
    bool found(true);
    quint32 objIdNum = objIdToNum(destinedObject);
    if (BacnetObjectTypeNS::Device !=  destinedObject.objectType) { //this is not a device, we have to find it's device object
        objIdNum = _objectDeviceMapper.findEntry(objIdNum, &found);//substitute locally objectIdNum with it's device number
    }

    if (found) { //Here, if foud is true, then objIdNum is the device instance. If so, try to find device address
        const mappingEntry &re = _devicesRoutingTable.findEntry(objIdNum, &found);//note that here objIdNum is id of the device
        if (found) {
            sendUnconfirmed(re.address, source, *data, serviceChoice);
            delete data;
            return found;
        }
    }


    /*being here means that either we have not sufficient information - if we had objectId, then objIdNum is pointing to it's device.
      Otherwise objIdNum is the object to be looked for itself. Create wrapper for a BacnetServiceData and send discovery request (whois or whohas -
      type depends on the objId Num value)
    */
    UnconfirmedDiscoveryWrapper *udw = new UnconfirmedDiscoveryWrapper(objIdNum, source, data, serviceChoice);
    _awaitingDiscoveries.insertMulti(objIdNum, udw);
    discover(objIdNum);

    return found;
}

void BacnetApplicationLayerHandler::sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    _tsm->sendUnconfirmed(destination, source, data, serviceChoice);
}

bool BacnetApplicationLayerHandler::send(const Bacnet::ObjectIdStruct &destinedObject, BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend)
{
    bool found(false);
    quint32 objIdNum = objIdToNum(destinedObject);
    if (BacnetObjectTypeNS::Device !=  destinedObject.objectType) { //this is not a device, we have to find it's device object
        objIdNum = _objectDeviceMapper.findEntry(objIdNum, &found);//substitute locally objectIdNum with it's device number
    }

    Q_ASSERT(InvalidInstanceNumber != numToObjId(objIdNum).instanceNum);
    const mappingEntry &re = _devicesRoutingTable.findEntry(objIdNum, &found);//note that here objIdNum is id of the device
    if (found) {
        send(re.address, sourceAddress, serviceToSend);
        return found;
    }

    /*being here means that either we have not sufficient information - if we had objectId, then objIdNum is pointing to it's device.
      Otherwise objIdNum is the object to be looked for itself. Create wrapper for a BacnetServiceData and send discovery request (whois or whohas -
      type depends on the objId Num value)
    */
    ConfirmedDiscoveryWrapper *cdw = new ConfirmedDiscoveryWrapper(objIdNum, sourceAddress, serviceToSend);
    _awaitingDiscoveries.insertMulti(objIdNum, cdw);
    discover(objIdNum);

    return found;
}

bool BacnetApplicationLayerHandler::send(const BacnetAddress &destination, BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend)
{
    _awaitingConfirmedServices.append(serviceToSend);
    return _tsm->send(destination, sourceAddress, serviceToSend);
}

void BacnetApplicationLayerHandler::indication(quint8 *data, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr)
{
    //is it really intended for us?
    Bacnet::BacnetDeviceObject *device(0);
    if (destAddr.isGlobalBroadcast() || destAddr.isRemoteBroadcast()){
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

void BacnetApplicationLayerHandler::discover(quint32 objectId, bool forceToHave)
{
    BacnetAddress fromAddress = _externalHandler->oneOfAddresses();
    if (!fromAddress.isAddrInitialized()) {
        Q_ASSERT(false);
        qDebug("%s : External objects handler has given uninitialized address!", __PRETTY_FUNCTION__);
        return;
    }
    BacnetAddress bCastAddr;
    bCastAddr.setGlobalBroadcast();
    ObjectIdStruct obId = numToObjId(objectId);
    if (BacnetObjectTypeNS::Device == obId.objectType) {//looking for a device. Issue Who is.
        WhoIsServiceData whoIsServiceData(objectId);
        _tsm->sendUnconfirmed(bCastAddr, fromAddress, whoIsServiceData, BacnetServicesNS::WhoIs);
    } else { //looking for an object, send who has.
        WhoHasServiceData whoHasServiceData(objectId);
        _tsm->sendUnconfirmed(bCastAddr, fromAddress, whoHasServiceData, BacnetServicesNS::WhoHas);
    }

    if (forceToHave)
        _awaitingDiscoveries.insertMulti(objectId, 0);
}

void BacnetApplicationLayerHandler::registerObject(ObjectIdentifier &devId, ObjectIdentifier &objId)
{
    _objectDeviceMapper.addOrUpdatemappingEntry(objId.objectIdNum(), devId.objectIdNum(), true);
}

void BacnetApplicationLayerHandler::registerObject(BacnetAddress &devAddress, ObjectIdentifier &devId, ObjectIdentifier &objId, QString &objName)
{
    ObjIdNum devNum = devId.objectIdNum();
    ObjIdNum objNum = objId.objectIdNum();
    ObjectIdStruct check = numToObjId(devNum);
    Q_ASSERT(check.instanceNum == devId.instanceNumber());
    Q_ASSERT(check.objectType == devId.type());
    qDebug("%s : Gotten response from device 0x%x, ovject 0x%x of name %s", __PRETTY_FUNCTION__, devNum, objNum, qPrintable(objName));

    bool isResponseForUs(false);

    QHash<ObjIdNum, DiscoveryWrapper*>::Iterator it = _awaitingDiscoveries.begin();
    QHash<ObjIdNum, DiscoveryWrapper*>::Iterator itEnd = _awaitingDiscoveries.end();

    while (it != itEnd) {
        if ( (it.key() == devNum) || (it.key() == objNum) ) { //it is a request issued as a response to our who-has request
            if (it.key() == objNum)
                isResponseForUs = true;
            if (0 != it.value()) //it could be zero, when the appliaction layer was called to resolve address of the object without any data to be send.
                (*it)->discoveryFinished(this, devAddress);
            it = _awaitingDiscoveries.erase(it);
            delete it.value();//the wrapper is not needed anymore. The data will be deleted along (if the wrapper didn't do something other with it already).
        } else
            ++it;
    }

    _objectDeviceMapper.addOrUpdatemappingEntry(objNum, devNum, isResponseForUs);
    /**
      If device was not in the devices list, add it. However, remember we don't have full information about the device - we insert some predicted defaults, which could be ok.
      To correct it, issue who-is and for a time being use those defaults.
      */
    if (!_devicesRoutingTable.addOrUpdatemappingEntry(devAddress, devNum, ApduMaxSize, SegmentedNOT, true, false)) {
        _awaitingDiscoveries.insertMulti(devNum, 0);
        discover(devNum);
    }
}

void BacnetApplicationLayerHandler::registerDevice(BacnetAddress &devAddress, Bacnet::ObjectIdentifier &devId, quint32 maxApduSize, BacnetSegmentation segmentationType, quint32 vendorId)
{
    ObjIdNum devNum = devId.objectIdNum();
    qDebug("%s : Gotten response from device 0x%x, and vendor id 0x%x", __PRETTY_FUNCTION__, devNum, vendorId);

    bool isResponseForUs(false);

    QHash<ObjIdNum, DiscoveryWrapper*>::Iterator it = _awaitingDiscoveries.begin();
    QHash<ObjIdNum, DiscoveryWrapper*>::Iterator itEnd = _awaitingDiscoveries.end();

    while (it != itEnd) {
        if (it.key() == devNum) { //it is a request issued as a response to our who-is request
            if (it.key() == devNum)
                isResponseForUs = true;
            if (0 != it.value()) //it could be zero, when the appliaction layer was called to resolve address of the object without any data to be send.
                (*it)->discoveryFinished(this, devAddress);
            it = _awaitingDiscoveries.erase(it);
            delete it.value();
        } else
            ++it;
    }

    _devicesRoutingTable.addOrUpdatemappingEntry(devAddress, devNum, maxApduSize, segmentationType, true, isResponseForUs);//force update, since this is for sure fine quality information!
}


ExternalObjectsHandler *BacnetApplicationLayerHandler::externalHandler()
{
    return _externalHandler;
}

InternalObjectsHandler *BacnetApplicationLayerHandler::internalHandler()
{
    return _internalHandler;
}

void BacnetApplicationLayerHandler::timerEvent(QTimerEvent *)
{
    //Check discovery services
    QHash<ObjIdNum, DiscoveryWrapper*>::Iterator it = _awaitingDiscoveries.begin();
    QHash<ObjIdNum, DiscoveryWrapper*>::Iterator itEnd = _awaitingDiscoveries.end();

    DiscoveryWrapper::Action action;
    while (it != itEnd) {
        if (0 != it.value()) { //there is some service pending for it.
            action = (*it)->handleTimeout(this);
            if (DiscoveryWrapper::DeleteMe == action) {
                it = _awaitingDiscoveries.erase(it);

                delete (*it);
            } else {
                Q_ASSERT(DiscoveryWrapper::LeaveMeInQueue == action);
                ++it;
            }
        } else { //that was us (app layer), who called it.
            it = _awaitingDiscoveries.erase(it);
        }
    }
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
#include "bacnetbipaddress.h"
#include "externalobjectreadstrategy.h"
#include "externalobjectwritestrategy.h"
#include "covanswerer.h"
#include "rpanswerer.h"
#include "wprequester.h"
#include "wpacknowledger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataModel::instance();

    BacnetNetworkLayerHandler *netHandler = new BacnetNetworkLayerHandler();
    BacnetApplicationLayerHandler *appHandler = new BacnetApplicationLayerHandler(netHandler);

    InternalObjectsHandler *intHandler = appHandler->internalHandler();
    ExternalObjectsHandler *extHandler = appHandler->externalHandler();

    InternalAddress extObjHandlerAddress = 32;
    extHandler->addRegisteredAddress(extObjHandlerAddress);

    QVariant test;
    test.setValue((double)72.3);

    BacnetAddress srcAddr;
    QHostAddress sa("192.168.1.70");
    BacnetBipAddressHelper::setMacAddress(sa, 12, &srcAddr);

    InternalAddress intAddr = 123;
    appHandler->externalHandler()->addRegisteredAddress(intAddr);

    quint32 destAddrInt(0x00000001);
    BacnetAddress destAddr = BacnetInternalAddressHelper::toBacnetAddress(destAddrInt);

    AsynchOwner *proto2 = new AsynchOwner();
    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Double);
    subject->setValue(test);
    proto2->addProperty(subject);

    BacnetDeviceObject *device = new BacnetDeviceObject(1, destAddr);
    HelperCoder::printArray(destAddr.macPtr(), destAddr.macAddrLength(), "Device 1 address");
    device->setObjectName("BacnetTestDevice");
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);
    ProxyInternalProperty propProxy(obs, AppTags::Real, QVariant::Double, device);
    device->addProperty(BacnetPropertyNS::PresentValue, &propProxy);
    intHandler->addDevice(device->address(), device);

    PropertySubject *subject2 = DataModel::instance()->createProperty(2, QVariant::Double);
    subject2->setValue(test);
    proto2->addProperty(subject2);

    PropertyObserver *obs2 = DataModel::instance()->createPropertyObserver(2);
    BacnetObject *aio = new BacnetObject(BacnetObjectTypeNS::AnalogInput, 5, device);
    //    AnalogInputObject *aio = new AnalogInputObject(5, device);
    aio->setObjectName("OATemp");
    ProxyInternalProperty propProxy2(obs2, AppTags::Real, QVariant::Double, aio);
    aio->addProperty(BacnetPropertyNS::PresentValue, &propProxy2);

    quint32 addr(0x00000003);
    BacnetAddress bAddr;
    BacnetInternalAddressHelper::macAddressFromRaw((quint8*)&addr, &bAddr);
    BacnetDeviceObject *device1 = new BacnetDeviceObject(8, bAddr);
    intHandler->addDevice(device1->address(), device1);
    device1->setObjectName("BestDeviceEver");

    //    BacnetObject *aio1 = new BacnetObject(BacnetObjectTypeNS::AnalogInput, 3, device1);
    ////    AnalogInputObject *aio1 = new AnalogInputObject(3, device1);
    //    aio1->setObjectName("OATemp");

    //    PropertySubject *extSubject = DataModel::instance()->createProperty(3, QVariant::Double);
    //    extHandler->addMappedProperty(extSubject, BacnetObjectType::AnalogValue << 22 | 0x01,
    //                                  BacnetProperty::PresentValue, ArrayIndexNotPresent,
    //                                  0x00000001,
    //                                  BacnetExternalObjects::Access_ReadRequest);

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

//            BacnetAddress broadAddr;
//            broadAddr.setGlobalBroadcast();
    ////        bHndlr->getBytes(whoHasService, sizeof(whoHasService), srcAddr, broadAddr);

    //        //WHO HAS - object id is known
    //        quint8 whoHasService2[] = {
    //            0x10,
    //            0x07,
    //            0x2c,
    //            0x00, 0x00, 0x00, 0x05
    //        };
    //        appHandler->indication(whoHasService2, sizeof(whoHasService2), srcAddr, broadAddr);

//    //I-HAVE
//    quint8 iHaveData[] = {
//        0x10,
//        0x01,
//        0xc4,
//        0x02, 0x00, 0x00, 0x08,
//        0xc4,
//        0x00, 0x00, 0x00, 0x03,
//        0x75,
//        0x07,
//        0x00,
//        0x4f, 0x41, 0x54, 0x65, 0x6d, 0x70
//    };
//    const quint16 iHaveDataSize = sizeof(iHaveData);
//    BacnetAddress broadAddr;
//    broadAddr.setGlobalBroadcast();

//    appHandler->indication(iHaveData, iHaveDataSize, srcAddr, broadAddr);

//    //I-AM
//    quint8 iAmData[] = {
//        0x10,
//        0x00,
//        0xc4,
//        0x02, 0x00, 0x00, 0x01,
//        0x22,
//        0x01, 0xe0,
//        0x91,
//        0x01,
//        0x21,
//        0x63
//    };
//    const quint16 iAmDataSize = sizeof(iAmData);
//    BacnetAddress broadAddr;
//    broadAddr.setGlobalBroadcast();

//    appHandler->indication(iAmData, iAmDataSize, srcAddr, broadAddr);

///////////////////////////////////////////////////////////////////
/////////////////ExternalObjectsHndlr tests////////////////////////
///////////////////////////////////////////////////////////////////

    BacnetAddress covDeviceAddress(srcAddr);
    ObjectIdentifier covDeviceIdentifier(BacnetObjectTypeNS::Device, 2);

//#define EXT_COV_TEST
#ifdef EXT_COV_TEST
    //To run COV test with externalObjHandler uncomment //#define EXT_COV_TEST in TSM and Externalobjects handler
    ObjectIdentifier covPropertyIdentifier(BacnetObjectTypeNS::AnalogInput, 10);

    PropertySubject *covPropSubject = DataModel::instance()->createProperty(5, (QVariant::Type)QMetaType::Float);

    appHandler->registerObject(covDeviceIdentifier, covPropertyIdentifier);
    appHandler->registerDevice(covDeviceAddress, covDeviceIdentifier, ApduMaxSize, SegmentedNOT, 99);

    CovReadStrategy *covStrategy = new CovReadStrategy(60000, true);
    covStrategy->setHasIncrement(true, 1.0);
    extHandler->addMappedProperty(covPropSubject, covPropertyIdentifier.objectIdNum(), BacnetPropertyNS::PresentValue, ArrayIndexNotPresent,
                                  covStrategy);

    BacnetAddress extHandlerAddress = extHandler->oneOfAddresses();
    CovAnswerer *covAns = new CovAnswerer(15, covDeviceAddress, extHandlerAddress, appHandler);
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), covAns, SLOT(answer()));
    timer.start(987);
#endif
#undef EXT_COV_TEST

//#define EXT_RP_TEST
#ifdef EXT_RP_TEST
    //to make it work, define EXT_RP_TEST in BacnetTsm2.cpp
    ObjectIdentifier rpPropertyIdentifier(BacnetObjectTypeNS::AnalogInput, 5);

    PropertySubject *rpPropSubject = DataModel::instance()->createProperty(5, (QVariant::Type)QMetaType::Float);

    appHandler->registerObject(covDeviceIdentifier, rpPropertyIdentifier);
    appHandler->registerDevice(covDeviceAddress, covDeviceIdentifier, ApduMaxSize, SegmentedNOT, 99);

    SimpleWithTimeReadStrategy *creadStrategy = new SimpleWithTimeReadStrategy(60000);
    extHandler->addMappedProperty(rpPropSubject, rpPropertyIdentifier.objectIdNum(), BacnetPropertyNS::PresentValue, ArrayIndexNotPresent,
                                  creadStrategy);

    BacnetAddress extHandlerAddress = extHandler->oneOfAddresses();
    RpAnswerer *rpAns = new RpAnswerer(1, covDeviceAddress, extHandlerAddress, appHandler);
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), rpAns, SLOT(answer()));
    timer.start(987);
#endif
#undef EXT_RP_TEST

//#define EXT_WP_TEST
#ifdef EXT_WP_TEST
    //to make it work, define EXT_RP_TEST in BacnetTsm2.cpp
    ObjectIdentifier wpPropertyIdentifier(BacnetObjectTypeNS::AnalogValue, 1);

    PropertySubject *wpPropSubject = DataModel::instance()->createProperty(5, (QVariant::Type)QMetaType::Float);
    Property *wpObserver = DataModel::instance()->createPropertyObserver(5);

    appHandler->registerObject(covDeviceIdentifier, wpPropertyIdentifier);
    appHandler->registerDevice(covDeviceAddress, covDeviceIdentifier, ApduMaxSize, SegmentedNOT, 99);

//    SimpleWithTimeReadStrategy *creadStrategy = new SimpleWithTimeReadStrategy(60000);
    SimpleReadStrategy *creadStrategy = new SimpleReadStrategy();
    ExternalObjectWriteStrategy *writeStrategy = new ExternalObjectWriteStrategy();
    extHandler->addMappedProperty(wpPropSubject, wpPropertyIdentifier.objectIdNum(), BacnetPropertyNS::PresentValue, ArrayIndexNotPresent,
                                  creadStrategy, writeStrategy);

    BacnetAddress extHandlerAddress = extHandler->oneOfAddresses();
    QVariant value = (float)180.0;
    WpRequester *wpReq = new WpRequester(wpObserver, value);

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), wpReq, SLOT(writeValue()));
    timer.start(987);

    QTimer timer2;
    timer2.setSingleShot(true);
    WpAcknowledger *wpAcknowledger = new WpAcknowledger(0x00, covDeviceAddress, extHandlerAddress, appHandler);
    QObject::connect(&timer2, SIGNAL(timeout()), wpAcknowledger, SLOT(answer()));
    timer2.start(1500);
#endif


    return a.exec();
}

#endif
