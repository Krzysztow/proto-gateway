#include "externalobjectshandler.h"

#include "cdm.h"
#include "bacnettsm2.h"
#include "bacnetcommon.h"
#include "bacnetdataabstract.h"
#include "bacnetreadpropertyack.h"
#include "error.h"
#include "writepropertyservicedata.h"
#include "bacnetreadpropertyservicehandler.h"
#include "bacnetwritepropertyservicehandler.h"
#include "bacnetdefaultobject.h"
#include "bacnetapplicationlayer.h"
#include "externalpropertymapping.h"
#include "subscribecovservicedata.h"
#include "subscribecovservicehandler.h"
#include "externalobjectreadstrategy.h"

using namespace Bacnet;

ExternalObjectsHandler::ExternalObjectsHandler(BacnetApplicationLayerHandler *appLayer):
    _appLayer(appLayer),
    _lastProcIdValueUsed(0)
{
}

ExternalObjectsHandler::~ExternalObjectsHandler()
{

}

void ExternalObjectsHandler::addMappedProperty(PropertySubject *property, quint32 objectId,
                                               BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx,
                                               ExternalObjectReadStrategy *readStrategy)
{
    Q_CHECK_PTR(property);
    if (0 == property) {
        qWarning("Property for object %d not added, since not allocated!", objectId);
        return;
    }

    if (_mappingTable.contains(property)) {
        qWarning("There are two same properties added!");
        Q_ASSERT(false);
        return;
    }

    property->setOwner(this);
    _mappingTable.insert(property, new ExternalPropertyMapping(property, readStrategy, propertyId, propertyArrayIdx, objectId));
}

ExternalPropertyMapping *ExternalObjectsHandler::mappingEntry(::Property *property)
{
    return _mappingTable.value(property);
}


void ExternalObjectsHandler::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(actionResult);
    Q_ASSERT_X(false, "ExternalObjectsHandler::asynchActionFinished()",
               "This function should never be called, since it owns only PropertySubjects instances.");
}

void ExternalObjectsHandler::propertyValueChanged(Property *property)
{
    Q_UNUSED(property);
    Q_ASSERT_X(false, "ExternalObjectsHandler::propertyValueChanged();",
               "This function should never be called, since it owns only PropertySubjects instances.");
}


int ExternalObjectsHandler::getPropertyRequested(::PropertySubject *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    if (0 == toBeGotten)
        return Property::UnknownError;

    ExternalPropertyMapping *rEntry = mappingEntry(toBeGotten);

    if (0 == rEntry)
        return Property::UnknownProperty;

    return readProperty(rEntry, toBeGotten);
}

int ExternalObjectsHandler::readProperty(ExternalPropertyMapping *readElement, bool askStrategy)
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        qDebug("ExternalObjectsHandler::readProperty() : can't send request, since we have no address.");
        return Property::UnknownError;
    }

    Q_CHECK_PTR(readElement);
    if (askStrategy) {
        Q_CHECK_PTR(readElement->readAccessStrategy);
        if (readElement->readAccessStrategy && readElement->readAccessStrategy->isValueReady())
            return Property::ResultOk;
    }

    //get new asynchronous id from data model
    int asynchId = DataModel::instance()->generateAsynchId();
    Q_ASSERT(asynchId >= 0);
    if (asynchId < 0) {
        qWarning("Can't generate asynchronous id.");
        return Property::UnknownError;
    }

    //! \todo Itroduce BacnetObjId class with conversion functions
    ReadPropertyServiceData *service =
            new ReadPropertyServiceData(numToObjId(readElement->objectId),
                                        readElement->propertyId, readElement->propertyArrayIdx);
    Q_CHECK_PTR(service);
    ExternalConfirmedServiceHandler *serviceHandler =
            new ReadPropertyServiceHandler(service, this, asynchId, readElement->mappedProperty);
    Q_CHECK_PTR(serviceHandler);

    ObjectIdStruct objId = numToObjId(readElement->objectId);
    BacnetAddress fromAddr = BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
    //the ownership isgiven to TSM - we will never delete it. We just use pointers as Asynchronous tokens.
    _appLayer->send(objId, fromAddr, BacnetServicesNS::ReadProperty, serviceHandler, 1000);

    return asynchId;
}

void ExternalObjectsHandler::handleResponse(ExternalConfirmedServiceHandler *act, bool ok)
{
    //    RequestInfo ri = _bacnetPendingRequests.take(act);
    //    Q_CHECK_PTR(ri.concernedProperty);
    //    Q_ASSERT(ri.asynchId > 0);

    //    bool found;
    //    BacnetExternalObjects::ExternalRoutingElement rEntry = mappingEntry(ri.concernedProperty, &found);
    //    Q_ASSERT(found);
    //    if (!found) {
    //        qWarning("ExternalObjectsHandler::handleResponse() property not found");
    //        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
    //        return;
    //    }

    //    if (ri.reqType == RequestWrite) {
    //        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::ResultOk);
    //    } else {
    //        Q_ASSERT(false);
    //    }
}

void ExternalObjectsHandler::handleResponse(ExternalConfirmedServiceHandler *act,
                                            BacnetReadPropertyAck &rp)
{
    //    if (!_bacnetPendingRequests.contains(act))//this could happen, if the internal timeout has occured
    //        return;

    //    RequestInfo ri = _bacnetPendingRequests.take(act);
    //    Q_CHECK_PTR(ri.concernedProperty);
    //    Q_ASSERT(ri.asynchId > 0);

    //    bool found;
    //    BacnetExternalObjects::ExternalRoutingElement rEntry = mappingEntry(ri.concernedProperty, &found);
    //    Q_ASSERT(found);
    //    if (!found) {
    //        qWarning("ExternalObjectsHandler::handleResponse() property not found");
    //        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
    //        return;
    //    }

    //    BacnetDataInterfaceShared value = rp._data;
    //    QVariant internalValue = value->toInternal();
    //    ri.concernedProperty->setValueSilent(internalValue);
    //    ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::ResultOk);
}

void ExternalObjectsHandler::handleError(ExternalConfirmedServiceHandler *act, Error &error)
{
    //    RequestInfo ri = _bacnetPendingRequests.take(act);
    //    if (!_bacnetPendingRequests.contains(act)) {//this could happen, if the internal timeout has occured
    //        ri.asynchId = 0;
    //        ri.concernedProperty = 0;
    //        ri.reqType = RequestRead;
    //    } else {
    //        RequestInfo ri = _bacnetPendingRequests.take(act);
    //        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
    //    }

    //    bool found;
    //    BacnetExternalObjects::ExternalRoutingElement rElem = mappingEntry(ri.concernedProperty, &found);
    //    Q_ASSERT(found);
    //    if (!found)
    //    {
    //        qDebug("ExternalObjectsHandler::handleError() - routing entry not found!");
    //        return;
    //    }

    //    qDebug("ExternalObjectsHandler::handleError() - Bacnet Error (class: %d, %d), while reading from object %d property %d",
    //           error.errorClass, error.errorCode, rElem._deviceIdentifier, rElem._objectIdentifier);
}

void ExternalObjectsHandler::handleAbort(ExternalConfirmedServiceHandler *act,  quint8 abortReason)
{
    //    RequestInfo ri = _bacnetPendingRequests.take(act);
    //    if (!_bacnetPendingRequests.contains(act)) {//this could happen, if the internal timeout has occured
    //        ri.asynchId = 0;
    //        ri.concernedProperty = 0;
    //        ri.reqType = RequestRead;
    //    } else {
    //        RequestInfo ri = _bacnetPendingRequests.take(act);
    //        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
    //    }

    //    bool found;
    //    BacnetExternalObjects::ExternalRoutingElement rElem = mappingEntry(ri.concernedProperty, &found);
    //    Q_ASSERT(found);
    //    if (!found)
    //    {
    //        qDebug("ExternalObjectsHandler::handleAbort() - routing entry not found!");
    //        return;
    //    }

    //    qDebug("ExternalObjectsHandler::handleAbort() - Bacnet Abort (%d), while %s from object %d property %d",
    //           abortReason,
    //           (ri.reqType == RequestWrite) ? "write" : ((ri.reqType == RequestRead) ? "read" : "cov subscription"),
    //           rElem._deviceIdentifier, rElem._objectIdentifier);
}

bool ExternalObjectsHandler::isRegisteredAddress(InternalAddress &address)
{
    return _registeredAddresses.contains(address);
}

void ExternalObjectsHandler::addRegisteredAddress(InternalAddress &address)
{
    if (!_registeredAddresses.contains(address))
        _registeredAddresses.append(address);
}

void ExternalObjectsHandler::removeRegisteredAddress(InternalAddress &address)
{
    _registeredAddresses.removeOne(address);
    Q_ASSERT(!_registeredAddresses.contains(address));
}

int ExternalObjectsHandler::setPropertyRequested(::PropertySubject *toBeSet, QVariant &value)
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        qDebug("ExternalObjectsHandler::setPropertyRequest() : can't send request, since we have no address.");
        return Property::UnknownError;
    }
    Q_CHECK_PTR(toBeSet);
    if (0 == toBeSet)
        return Property::UnknownError;

    ExternalPropertyMapping *rEntry = _mappingTable.value(toBeSet);

    if (0 == rEntry)
        return Property::UnknownProperty;
    Q_ASSERT(rEntry->isValid());

    //rEntry->writeProperty(

    int asynchId = DataModel::instance()->generateAsynchId();
    Q_ASSERT(asynchId >= 0);
    if (asynchId < 0) {
        qWarning("Can't generate asynchronous id.");
        return Property::UnknownError;
    }

    ObjectIdentifier objectId(rEntry->objectId);
    Bacnet::BacnetDataInterface *writeData = BacnetDefaultObject::createDataForObjectProperty(objectId.type(), rEntry->propertyId, rEntry->propertyArrayIdx);
    Q_CHECK_PTR(writeData);
    if (0 == writeData) {
        qWarning("Can't create appropriate value isntance for %d, %d", objectId.type(), rEntry->propertyId);
        return Property::UnknownError;
    }

    if (!writeData->setInternal(value)) {
        qDebug("ExternalObjectsHandler::setPropertyRequest() : Can't convert variant type %d, to bacnet type %d",
               value.type(), writeData->typeId());
        delete writeData;
        return Property::UnknownError;
    }

    WritePropertyServiceData *serviceData =
            new WritePropertyServiceData(objectId, rEntry->propertyId, writeData, rEntry->propertyArrayIdx);
    Q_CHECK_PTR(serviceData);
    BacnetWritePropertyServiceHandler *serviceHandler =
            new BacnetWritePropertyServiceHandler(serviceData, this, toBeSet, asynchId);
    Q_CHECK_PTR(serviceHandler);

    BacnetAddress fromAddr = BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
    //the ownership isgiven to AppLayer. We just use pointers as Asynchronous tokens.
    _appLayer->send(objectId.objIdStruct(), fromAddr, BacnetServicesNS::WriteProperty, serviceHandler, 1000);

    return asynchId;
}

//Bacnet::WritePropertyServiceData::WritePropertyServiceData(Bacnet::ObjectIdStruct&, BacnetPropertyNS::Identifier&, Bacnet::BacnetDataInterface*&, quint32&)’
//Bacnet::WritePropertyServiceData::WritePropertyServiceData(Bacnet::ObjectIdentifier&, BacnetPropertyNS::Identifier, Bacnet::BacnetDataInterface*, quint32)

BacnetAddress Bacnet::ExternalObjectsHandler::oneOfAddresses()
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        BacnetAddress uninitAddr;
        return uninitAddr;
    } else
        return BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
}

bool ExternalObjectsHandler::makeCovSubscription(ExternalPropertyMapping *readElement, bool isConfirmedCovSubscription, quint32 lifetime_s, CovReadStrategy *covStreategy)
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        qDebug("%s : cant make subscription, address empty", __PRETTY_FUNCTION__);
        return false;
    }

    Q_CHECK_PTR(readElement);
    if (0 == readElement)
        return false;
    Q_ASSERT(readElement->isValid());

    quint8 generateProcId = insertToSubscribeCovs(isConfirmedCovSubscription, readElement, covStreategy);
    if (isConfirmedCovSubscription && (0 == generateProcId)) {
        qDebug("%s : Couldn't generate Cov Process Id key.", __PRETTY_FUNCTION__);
        return false;
    }

    SubscribeCOVServiceData *serviceData =
            new SubscribeCOVServiceData(generateProcId, readElement->objectId, isConfirmedCovSubscription, lifetime_s);
    Q_CHECK_PTR(serviceData);

    SubscribeCovServiceHandler *serviceHandler =
            new SubscribeCovServiceHandler(serviceData);
    Q_CHECK_PTR(serviceHandler);

    BacnetAddress fromAddr = BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
    ObjectIdentifier objectId(readElement->objectId);
    //the ownership isgiven to AppLayer. We just use pointers as Asynchronous tokens.
    _appLayer->send(objectId.objIdStruct(), fromAddr, BacnetServicesNS::SubscribeCOVProperty, serviceHandler, 1000);

    return true;
}

quint32 ExternalObjectsHandler::insertToSubscribeCovs(bool confirmed, ExternalPropertyMapping *propertyMapping, CovReadStrategy *readStrategy)
{
    if (confirmed) {
        _subscribedCovs.insertMulti(0, qMakePair(propertyMapping, readStrategy));
        return 0;
    }

    //look for the unique number
    quint32 newId(_lastProcIdValueUsed + 1);
    quint32 guard(0);
    while (guard < MaximumConfirmedSubscriptions) {
        if (_subscribedCovs.contains(newId))
                ++newId;
        else {
            _lastProcIdValueUsed = newId;
            return _lastProcIdValueUsed;
        }
    }

    return 0;
}
