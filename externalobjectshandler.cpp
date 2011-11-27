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

using namespace Bacnet;

ExternalObjectsHandler::ExternalObjectsHandler(BacnetApplicationLayerHandler *appLayer):
    _appLayer(appLayer)
{
}

ExternalObjectsHandler::~ExternalObjectsHandler()
{

}

void ExternalObjectsHandler::addMappedProperty(Property *property, quint32 objectId,
                                               BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx,
                                               quint32 deviceId,
                                               ExternalPropertyMapping::ReadAccessType type)
{
    Q_CHECK_PTR(property);
    if (0 == property) {
        qWarning("Property for object %d not added, since not allocated!", objectId);
        return;
    }

    bool foundOne;
    mappingEntry(property, &foundOne);
    if (foundOne) {
        qWarning("There are two same properties added!");
        Q_ASSERT(false);
    }

    property->setOwner(this);

    ExternalPropertyMapping propMapping(property, type, propertyId, propertyArrayIdx, objectId);

    //if type is read - COV -> subscribe & first read value.

    _routingTable.append(propMapping);
}

ExternalPropertyMapping &ExternalObjectsHandler::mappingEntry(::Property *property, bool *found)
{
    QList<ExternalPropertyMapping>::Iterator entryIt = _routingTable.begin();
    QList<ExternalPropertyMapping>::Iterator rtEnd = _routingTable.end();

    for (; entryIt != rtEnd; ++entryIt) {
        if (entryIt->mappedProperty == property) {
            if (0 != found) *found = true;
            return *(entryIt);
        }
    }

    if (0 != found) *found = false;
    static ExternalPropertyMapping wrongEntry;
    return wrongEntry;
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

    bool isFound;
    ExternalPropertyMapping &rEntry = mappingEntry(toBeGotten, &isFound);

    if (!isFound)
        return Property::UnknownProperty;

    switch (rEntry.readAccessType) {
    case (ExternalPropertyMapping::ReadCov_Uninitialized):  //fall through
        //issue cov subscription request
    case (ExternalPropertyMapping::ReadRP): {
        return issueReadPropertyRequest(rEntry, toBeGotten);
        break;
    }
    case (ExternalPropertyMapping::ReadCov_Confirmed):      //fall through
    case (ExternalPropertyMapping::ReadCov_Unconfirmed):
    {
        //the property is ready to be read
        return Property::ResultOk;
        break;
    }
    default:
        Q_ASSERT(false);
        return Property::UnknownError;
    }
}

int ExternalObjectsHandler::issueReadPropertyRequest(ExternalPropertyMapping &readElement, PropertySubject *property)
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        qDebug("ExternalObjectsHandler::readProperty() : can't send request, since we have no address.");
        return Property::UnknownError;
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
            new ReadPropertyServiceData(numToObjId(readElement.objectId),
                                        readElement.propertyId, readElement.propertyArrayIdx);
    Q_CHECK_PTR(service);
    ExternalConfirmedServiceHandler *serviceHandler =
            new ReadPropertyServiceHandler(service, this, asynchId, property);
    Q_CHECK_PTR(serviceHandler);

    ObjectIdStruct objId = numToObjId(readElement.objectId);
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

    bool isFound;
    ExternalPropertyMapping &rEntry = mappingEntry(toBeSet, &isFound);

    if (!isFound || !rEntry.isValid())
        return Property::UnknownProperty;

    int asynchId = DataModel::instance()->generateAsynchId();
    Q_ASSERT(asynchId >= 0);
    if (asynchId < 0) {
        qWarning("Can't generate asynchronous id.");
        return Property::UnknownError;
    }

    ObjectIdentifier objectId(rEntry.objectId);
    Bacnet::BacnetDataInterface *writeData = BacnetDefaultObject::createDataForObjectProperty(objectId.type(), rEntry.propertyId, rEntry.propertyArrayIdx);
    Q_CHECK_PTR(writeData);
    if (0 == writeData) {
        qWarning("Can't create appropriate value isntance for %d, %d", objectId.type(), rEntry.propertyId);
        return Property::UnknownError;
    }

    if (!writeData->setInternal(value)) {
        qDebug("ExternalObjectsHandler::setPropertyRequest() : Can't convert variant type %d, to bacnet type %d",
               value.type(), writeData->typeId());
        delete writeData;
        return Property::UnknownError;
    }

    WritePropertyServiceData *serviceData =
            new WritePropertyServiceData(objectId, rEntry.propertyId, writeData, rEntry.propertyArrayIdx);
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
