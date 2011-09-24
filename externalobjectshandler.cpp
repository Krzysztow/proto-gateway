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

using namespace Bacnet;

ExternalObjectsHandler::ExternalObjectsHandler(BacnetTSM2 *tsm):
        _tsm(tsm)
{
}

void ExternalObjectsHandler::addMappedProperty(Property *property, quint32 objectId,
                                               BacnetProperty::Identifier propertyId, quint32 propertyArrayIdx,
                                               quint32 deviceId,
                                               BacnetExternalObjects::ReadAccessType type)
{
    Q_CHECK_PTR(property);
    if (0 == property) {
        qWarning("Property for object %d not added, since not allocated!", objectId);
        return;
    }

    bool foundOne;
    routingEntry(property, &foundOne);
    if (foundOne) {
        qWarning("There are two same properties added!");
        Q_ASSERT(false);
    }

    property->setOwner(this);

    BacnetExternalObjects::ExternalRoutingElement extObj = { property, deviceId, objectId,
                                                             propertyId, propertyArrayIdx,
                                                             type };

    //if type is read - COV -> subscribe & first read value.

    _routingTable.append(extObj);
}

BacnetExternalObjects::ExternalRoutingElement &ExternalObjectsHandler::routingEntry(::Property *property, bool *found)
{
    QVector<BacnetExternalObjects::ExternalRoutingElement>::Iterator entryIt = _routingTable.begin();
    QVector<BacnetExternalObjects::ExternalRoutingElement>::Iterator rtEnd = _routingTable.end();

    for (; entryIt != rtEnd; ++entryIt) {
        if (entryIt->_property == property) {
            if (0 != found) *found = true;
            return *(entryIt);
        }
    }

    if (0 != found) *found = false;
    static BacnetExternalObjects::ExternalRoutingElement wrongEntry = {
        0, 0, BacnetObjectType::Undefined,
        BacnetProperty::UndefinedProperty, Bacnet::ArrayIndexNotPresent,
        BacnetExternalObjects::Access_COV
    };
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

void ExternalObjectsHandler::propertyValueChanged(PropertyObserver *property)
{
    Q_UNUSED(property);
    Q_ASSERT_X(false, "ExternalObjectsHandler::propertyValueChanged();",
               "This function should never be called, since it owns only PropertySubjects instances.");
}


int ExternalObjectsHandler::getPropertyRequest(::PropertySubject *toBeGotten)
{
    Q_CHECK_PTR(toBeGotten);
    if (0 == toBeGotten)
        return Property::UnknownError;

    bool isFound;
    BacnetExternalObjects::ExternalRoutingElement rEntry = routingEntry(toBeGotten, &isFound);

    if (!isFound)
        return Property::UnknownProperty;

    switch (rEntry._read) {
    case (BacnetExternalObjects::Access_COV_Uninitialized)://fall through
    case (BacnetExternalObjects::Access_ReadRequest):
        {
            return readProperty(rEntry, toBeGotten);
            break;
        }
    case (BacnetExternalObjects::Access_COV):
        {
            //the property is ready to be read
            return Property::ResultOk;
            break;
        }
    default:
        Q_ASSERT(false);
        return Property::UnknownError;
    }

    return Property::UnknownError;
}

int ExternalObjectsHandler::readProperty(BacnetExternalObjects::ExternalRoutingElement &readElement, ::PropertySubject *property)
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
            new ReadPropertyServiceData(numToObjId(readElement._objectIdentifier),
                                          readElement._propertyId, readElement._propertyArrayIdx);
    Q_CHECK_PTR(service);
    ReadPropertyServiceHandler *serviceHandler =
            new ReadPropertyServiceHandler(service, this);
    Q_CHECK_PTR(serviceHandler);

    RequestInfo ri = {asynchId, RequestRead, property};
    _bacnetPendingRequests.insert(serviceHandler, ri);
    ObjectIdStruct objId = numToObjId(readElement._deviceIdentifier);
    //the ownership isgiven to TSM - we will never delete it. We just use pointers as Asynchronous tokens.
    _tsm->send(objId, _registeredAddresses.first(), BacnetServices::ReadProperty, serviceHandler, 1000);

    return asynchId;
}

void ExternalObjectsHandler::handleResponse(BacnetConfirmedServiceHandler *act, bool ok)
{
    if (!_bacnetPendingRequests.contains(act))//this could happen, if the internal timeout has occured
        return;

    RequestInfo ri = _bacnetPendingRequests.take(act);
    Q_CHECK_PTR(ri.concernedProperty);
    Q_ASSERT(ri.asynchId > 0);

    bool found;
    BacnetExternalObjects::ExternalRoutingElement rEntry = routingEntry(ri.concernedProperty, &found);
    Q_ASSERT(found);
    if (!found) {
        qWarning("ExternalObjectsHandler::handleResponse() property not found");
        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
        return;
    }

    if (ri.reqType == RequestWrite) {
        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::ResultOk);
    } else {
        Q_ASSERT(false);
    }
}

void ExternalObjectsHandler::handleResponse(BacnetConfirmedServiceHandler *act,
                                            BacnetReadPropertyAck &rp)
{
    if (!_bacnetPendingRequests.contains(act))//this could happen, if the internal timeout has occured
        return;

    RequestInfo ri = _bacnetPendingRequests.take(act);
    Q_CHECK_PTR(ri.concernedProperty);
    Q_ASSERT(ri.asynchId > 0);

    bool found;
    BacnetExternalObjects::ExternalRoutingElement rEntry = routingEntry(ri.concernedProperty, &found);
    Q_ASSERT(found);
    if (!found) {
        qWarning("ExternalObjectsHandler::handleResponse() property not found");
        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
        return;
    }

    BacnetDataInterface *value = rp._data;
    QVariant internalValue = value->toInternal();
    ri.concernedProperty->setValueSilent(internalValue);
    ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::ResultOk);
}

void ExternalObjectsHandler::handleError(BacnetConfirmedServiceHandler *act, Error &error)
{
    RequestInfo ri = _bacnetPendingRequests.take(act);
    if (!_bacnetPendingRequests.contains(act)) {//this could happen, if the internal timeout has occured
        ri.asynchId = 0;
        ri.concernedProperty = 0;
        ri.reqType = RequestRead;
    } else {
        RequestInfo ri = _bacnetPendingRequests.take(act);
        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
    }

    bool found;
    BacnetExternalObjects::ExternalRoutingElement rElem = routingEntry(ri.concernedProperty, &found);
    Q_ASSERT(found);
    if (!found)
    {
        qDebug("ExternalObjectsHandler::handleError() - routing entry not found!");
        return;
    }

    qDebug("ExternalObjectsHandler::handleError() - Bacnet Error (class: %d, %d), while reading from object %d property %d",
           error.errorClass, error.errorCode, rElem._deviceIdentifier, rElem._objectIdentifier);
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

void ExternalObjectsHandler::handleAbort(BacnetConfirmedServiceHandler *act,  quint8 abortReason)
{
    RequestInfo ri = _bacnetPendingRequests.take(act);
    if (!_bacnetPendingRequests.contains(act)) {//this could happen, if the internal timeout has occured
        ri.asynchId = 0;
        ri.concernedProperty = 0;
        ri.reqType = RequestRead;
    } else {
        RequestInfo ri = _bacnetPendingRequests.take(act);
        ri.concernedProperty->asynchActionFinished(ri.asynchId, Property::UnknownError);
    }

    bool found;
    BacnetExternalObjects::ExternalRoutingElement rElem = routingEntry(ri.concernedProperty, &found);
    Q_ASSERT(found);
    if (!found)
    {
        qDebug("ExternalObjectsHandler::handleAbort() - routing entry not found!");
        return;
    }

    qDebug("ExternalObjectsHandler::handleAbort() - Bacnet Abort (%d), while %s from object %d property %d",
           abortReason,
           (ri.reqType == RequestWrite) ? "write" : ((ri.reqType == RequestRead) ? "read" : "cov subscription"),
           rElem._deviceIdentifier, rElem._objectIdentifier);
}

int ExternalObjectsHandler::setPropertyRequest(::PropertySubject *toBeSet, QVariant &value)
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
    BacnetExternalObjects::ExternalRoutingElement rEntry = routingEntry(toBeSet, &isFound);

    if (!isFound)
        return Property::UnknownProperty;

    ObjectIdStruct objectId = numToObjId(rEntry._objectIdentifier);

    Bacnet::BacnetDataInterface *writeData = BacnetDefaultObject::createDataForObjectProperty(objectId.objectType, rEntry._propertyId, rEntry._propertyArrayIdx);
    Q_CHECK_PTR(writeData);
    if (0 == writeData) {
        qWarning("Can't create appropriate value isntance for %d, %d", objectId.objectType, rEntry._propertyId);
        return Property::UnknownError;
    }

    if (!writeData->setInternal(value)) {
        qDebug("ExternalObjectsHandler::setPropertyRequest() : Can't convert variant type %d, to bacnet type %d",
               value.type(), writeData->typeId());
        delete writeData;
        return Property::UnknownError;
    }

    WritePropertyServiceData *serviceData =
            new WritePropertyServiceData(numToObjId(rEntry._objectIdentifier),
                                           rEntry._propertyId, writeData, rEntry._propertyArrayIdx);
    Q_CHECK_PTR(serviceData);
    BacnetWritePropertyServiceHandler *serviceHandler =
            new BacnetWritePropertyServiceHandler(serviceData, this);
    Q_CHECK_PTR(serviceHandler);

    int asynchId = DataModel::instance()->generateAsynchId();
    Q_ASSERT(asynchId >= 0);
    if (asynchId < 0) {
        delete serviceData;
        delete serviceHandler;
        delete writeData;
        qWarning("Can't generate asynchronous id.");
        return Property::UnknownError;
    }

    RequestInfo ri = {asynchId, RequestWrite, toBeSet};
    _bacnetPendingRequests.insert(serviceHandler, ri);
    ObjectIdStruct objId = numToObjId(rEntry._deviceIdentifier);
    //the ownership isgiven to TSM - we will never delete it. We just use pointers as Asynchronous tokens.
    _tsm->send(objId, _registeredAddresses.first(), BacnetServices::WriteProperty, serviceHandler, 1000);

    return asynchId;
}
