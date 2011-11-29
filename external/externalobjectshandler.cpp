#include "externalobjectshandler.h"

#include "cdm.h"
#include "bacnettsm2.h"
#include "bacnetcommon.h"
#include "bacnetdataabstract.h"
#include "bacnetreadpropertyack.h"
#include "error.h"
#include "writepropertyservicedata.h"
#include "bacnetwritepropertyservicehandler.h"
#include "bacnetdefaultobject.h"
#include "bacnetapplicationlayer.h"
#include "externalpropertymapping.h"
#include "subscribecovservicedata.h"
#include "subscribecovservicehandler.h"
#include "externalobjectreadstrategy.h"
#include "externalobjectwritestrategy.h"

using namespace Bacnet;

ExternalObjectsHandler::ExternalObjectsHandler(BacnetApplicationLayerHandler *appLayer):
    _interval_ms(DefaultInterval_ms),
    _lastProcIdValueUsed(0),
    _appLayer(appLayer)
{
}

ExternalObjectsHandler::~ExternalObjectsHandler()
{

}

void ExternalObjectsHandler::addMappedProperty(PropertySubject *property, Bacnet::ObjIdNum objectId, BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx,
                                               ExternalObjectReadStrategy *readStrategy, ExternalObjectWriteStrategy *writeStrategy)
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
    ExternalPropertyMapping *propMapping = new ExternalPropertyMapping(property, readStrategy, writeStrategy, propertyId, propertyArrayIdx, objectId);
    _mappingTable.insert(property, propMapping);

    tryToAdd(readStrategy, propMapping);
    tryToAdd(writeStrategy, propMapping);
}

void ExternalObjectsHandler::tryToAdd(ExternalTimeDepJob *job, ExternalPropertyMapping *correspondingMapping)
{
    if (0 != job && job->isPeriodic()) {
        Q_ASSERT(!_timeDependantJobs.contains(qMakePair(job, correspondingMapping)));
        _timeDependantJobs.append(qMakePair(job, correspondingMapping));

        if (!_timer.isActive() && !_timeDependantJobs.isEmpty())
            _timer.start(_interval_ms, this);
    }
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

    if ( (0 == rEntry) || (0 == rEntry->readAccessStrategy) ) {
        qDebug("%s : error on getting read access strategy.", __PRETTY_FUNCTION__);
        return Property::UnknownProperty;
    }

    Q_CHECK_PTR(rEntry->readAccessStrategy);
    return rEntry->readAccessStrategy->readProperty(rEntry, this, true);
}

bool ExternalObjectsHandler::send(ExternalConfirmedServiceHandler *serviceHandler, ObjIdNum destinedObject)
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        qDebug("ExternalObjectsHandler::readProperty() : can't send request, since we have no address.");
        return false;
    }

//    Q_CHECK_PTR(readElement);
//    if (askStrategy) {
//        Q_CHECK_PTR(readElement->readAccessStrategy);
//        if (readElement->readAccessStrategy && readElement->readAccessStrategy->isValueReady())
//            return Property::ResultOk;
//    }

//    //get new asynchronous id from data model
//    int asynchId(0);
//    if (generateId)
//        asynchId = DataModel::instance()->generateAsynchId();
//    Q_ASSERT(asynchId >= 0);
//    if (asynchId < 0) {
//        qWarning("Can't generate asynchronous id.");
//        return Property::UnknownError;
//    }

//    //! \todo Itroduce BacnetObjId class with conversion functions
//    ReadPropertyServiceData *service =
//            new ReadPropertyServiceData(numToObjId(readElement->objectId),
//                                        readElement->propertyId, readElement->propertyArrayIdx);
//    Q_CHECK_PTR(service);
//    ExternalConfirmedServiceHandler *serviceHandler =
//            new ReadPropertyServiceHandler(service, asynchId, readElement);
//    Q_CHECK_PTR(serviceHandler);

//    ObjectIdStruct objId = numToObjId(readElement->objectId);
    //the ownership isgiven to TSM - we will never delete it. We just use pointers as Asynchronous tokens.

    ObjectIdentifier objId(destinedObject);
    BacnetAddress fromAddr = BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
    return _appLayer->send(objId.objIdStruct(), fromAddr, BacnetServicesNS::ReadProperty, serviceHandler);
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

    if ( (0 == rEntry) || (0 == rEntry->writeStrategy) ) {
        qDebug("%s : error on getting write access strategy!", __PRETTY_FUNCTION__);
        return Property::UnknownProperty;
    }

    Q_ASSERT(rEntry->isValid());
    Q_CHECK_PTR(rEntry->writeStrategy);
    return rEntry->writeStrategy->writeProperty(rEntry, this, value, true);
}

//WritePropertyServiceData::WritePropertyServiceData(ObjectIdStruct&, BacnetPropertyNS::Identifier&, BacnetDataInterface*&, quint32&)’
//WritePropertyServiceData::WritePropertyServiceData(ObjectIdentifier&, BacnetPropertyNS::Identifier, BacnetDataInterface*, quint32)

BacnetAddress ExternalObjectsHandler::oneOfAddresses()
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        BacnetAddress uninitAddr;
        return uninitAddr;
    } else
        return BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
}

bool ExternalObjectsHandler::startCovSubscriptionProcess(ExternalPropertyMapping *propertyMapping, bool isConfirmedCovSubscription, quint32 lifetime_s, CovReadStrategy *covStreategy)
{
    Q_ASSERT(!_registeredAddresses.isEmpty());
    if (_registeredAddresses.isEmpty()) {
        qDebug("%s : cant make subscription, address empty", __PRETTY_FUNCTION__);
        return false;
    }

    Q_CHECK_PTR(propertyMapping);
    if (0 == propertyMapping)
        return false;
    Q_ASSERT(propertyMapping->isValid());

    quint8 generateProcId = insertToOrFindSubscribeCovs(propertyMapping, covStreategy);
    if (isConfirmedCovSubscription && (0 == generateProcId)) {
        qDebug("%s : Couldn't generate Cov Process Id key.", __PRETTY_FUNCTION__);
        return false;
    }

    SubscribeCOVServiceData *serviceData =
            new SubscribeCOVServiceData(generateProcId, propertyMapping->objectId, isConfirmedCovSubscription, true, lifetime_s,
                                        propertyMapping->propertyId, propertyMapping->propertyArrayIdx);
    if (covStreategy->hasIncrement())
        serviceData->setCovIncrement(covStreategy->incrementValue());

    Q_CHECK_PTR(serviceData);

    SubscribeCovServiceHandler *serviceHandler =
            new SubscribeCovServiceHandler(serviceData, this, propertyMapping, covStreategy);
    Q_CHECK_PTR(serviceHandler);

    BacnetAddress fromAddr = BacnetInternalAddressHelper::toBacnetAddress(_registeredAddresses.first());
    ObjectIdentifier objectId(propertyMapping->objectId);
    //the ownership isgiven to AppLayer. We just use pointers as Asynchronous tokens.
    _appLayer->send(objectId.objIdStruct(), fromAddr, BacnetServicesNS::SubscribeCOVProperty, serviceHandler, 1000);

    return true;
}

int ExternalObjectsHandler::insertToOrFindSubscribeCovs(ExternalPropertyMapping *propertyMapping, CovReadStrategy *readStrategy, int valueHint)
{
    int returnProcId(-1);
    //    if (!confirmed) {
    //        {
    //            //check if we are there already. QHash::find(key) returns iterator to the most recent element inserted. If there are others, their are accessible by incremenation.
    //            const int key = UnconfirmedProcIdValue;
    //            QHash<int, TCovMappinPair>::Iterator it = _subscribedCovs.find(key);
    //            QHash<int, TCovMappinPair>::Iterator itEnd = _subscribedCovs.end();
    //            for (; it != itEnd; ++it) {
    //                if (it.key() != UnconfirmedProcIdValue)
    //                    break;
    //                if ( (it->first == propertyMapping) &&
    //                     (it->second == readStrategy) ) {
    //                    returnProcId = UnconfirmedProcIdValue;
    //                    break;
    //                }
    //            }
    //        }

    //        //if we were not found, insert us
    //        if (returnProcId != UnconfirmedProcIdValue) {
    //            const int key = UnconfirmedProcIdValue;
    //            _subscribedCovs.insertMulti(key, qMakePair(propertyMapping, readStrategy));
    //            returnProcId = UnconfirmedProcIdValue;
    //        }
    //    } else {
    if (valueHint >= 0) {
        QHash<int, TCovMappinPair>::Iterator it = _subscribedCovs.find(valueHint);
        if ( (it->first == propertyMapping) && (it->second == readStrategy) )
            returnProcId = valueHint;
    }

    //we still have to look for the unique number
    if (returnProcId < 0) {
        quint32 newId(_lastProcIdValueUsed + 1);
        quint32 guard(0);
        while (guard < MaximumConfirmedSubscriptions) {
            if (_subscribedCovs.contains(newId))
                ++newId;
            else {
                _lastProcIdValueUsed = newId;
                returnProcId = _lastProcIdValueUsed;
                break;
            }
        }
//#define EXT_COV_TEST
#ifdef EXT_COV_TEST
#warning "For testing purposes set 18 (like in examples). REMOVE later!"
        Q_ASSERT(!_subscribedCovs.contains(18));
        returnProcId = 18;
#endif
        //the key was found, insert us
        Q_ASSERT(!_subscribedCovs.contains(returnProcId));
        _subscribedCovs.insert(returnProcId, qMakePair(propertyMapping, readStrategy));

        if ( (valueHint >= 0) && (returnProcId != valueHint) ) {//we have to take care, that we remove the valueHint if it represents CovReadStrategy
            QHash<int, TCovMappinPair>::Iterator checkIt = _subscribedCovs.find(valueHint);
            QHash<int, TCovMappinPair>::Iterator checkItEnd = _subscribedCovs.end();
            for (; checkIt != checkItEnd; ++checkIt) {
                if (checkIt.key() != valueHint)
                    break;
                if ( (checkIt->first == propertyMapping) && (checkIt->second == readStrategy) ) {
                    _subscribedCovs.erase(checkIt);
                    break;
                }

            }
        }
    }

    return returnProcId;

}

void ExternalObjectsHandler::covSubscriptionProcessFinished(int subscribeProcId, ExternalPropertyMapping *propertyMapping, CovReadStrategy *readStrategy, bool ok, bool isCritical)
{

    //    if (0 == subscribeProcId) {
    //        QHash<int, TCovMappinPair>::Iterator itEnd = _subscribedCovs.end();
    //        for (; it != itEnd; ++it) {
    //            if (it.key() != subscribeProcId) {
    //                it = _subscribedCovs.end();
    //                break;
    //            }
    //            if ( (propertyMapping == it->first) && (readStrategy == it->second) )
    //                break;
    //        }
    //    }

    Q_ASSERT(0 != subscribeProcId);
    QHash<int, TCovMappinPair>::Iterator it = _subscribedCovs.find(subscribeProcId);
    if (_subscribedCovs.end() == it) {
        qDebug("%s : subscription finished, but we got wrong parameters", __PRETTY_FUNCTION__);
        Q_ASSERT(false);
        return;
    }
    Q_ASSERT((propertyMapping == it->first) && (readStrategy == it->second));

    if (!ok)
        _subscribedCovs.erase(it);
    readStrategy->setSubscriptionInitiated(ok, subscribeProcId, isCritical);
}

void ExternalObjectsHandler::timerEvent(QTimerEvent *)
{
    //iterate over jobs and tell then the time has passed
    QList<TTimeDependantPair>::Iterator it = _timeDependantJobs.begin();
    QList<TTimeDependantPair>::Iterator itEnd = _timeDependantJobs.end();

    for (; it != itEnd; ++it) {
        if (it->first->timePassed(_interval_ms))
            it->first->doAction(it->second, this);
    }
}

#include "bacnetarrayvisitor.h"

void ExternalObjectsHandler::covValueChangeNotification(CovNotificationRequestData &data, bool isConfirmed, Error *error)
{
    QHash<int, TCovMappinPair>::Iterator it = _subscribedCovs.find(data._subscribProcess);
    if (_subscribedCovs.end() == it) {
        qDebug("%s : No such cov subscription!", __PRETTY_FUNCTION__);
        if (0 != error)
            error->setError(BacnetErrorNS::ClassServices, BacnetErrorNS::CodeOther);
        return;
    }

    Q_CHECK_PTR(it->first);
    Q_CHECK_PTR(it->second);


    ExternalPropertyMapping *mapping = it->first;
    CovReadStrategy *covStrategy = it->second;
    Q_ASSERT(_mappingTable.contains(mapping->mappedProperty));
    Q_ASSERT(_mappingTable[mapping->mappedProperty]->objectId == mapping->objectId);
    Q_ASSERT(_mappingTable[mapping->mappedProperty]->propertyArrayIdx == mapping->propertyArrayIdx);
    if (!_mappingTable.contains(mapping->mappedProperty)) {
        qDebug("%s : mapping not found!", __PRETTY_FUNCTION__);
        _subscribedCovs.erase(it);
        if (0 != error)
            error->setError(BacnetErrorNS::ClassServices, BacnetErrorNS::CodeOther);
        return;
    }

    if (data._monitoredObjectId.objectIdNum() != mapping->objectId) {//this is a notification being meant not for us!
        qDebug("%s : cov notification not for us, but for obj with id 0x%x", __PRETTY_FUNCTION__, data._initiatingDevObjtId.objectIdNum());
        Q_ASSERT(false);
        if (0 != error)
            error->setError(BacnetErrorNS::ClassServices, BacnetErrorNS::CodeOther);
        return;
    }

    QList<PropertyValueShared> valuesList = data._listOfValues.value();
    for (int i = 0; i < valuesList.count(); ++i) {
        PropertyValueShared &propValue = valuesList[i];
        if (propValue->_propertyId == mapping->propertyId) {
            BacnetDataInterfaceShared value;
            if ( (ArrayIndexNotPresent == propValue->_arrayIndex) &&
                 (ArrayIndexNotPresent != mapping->propertyArrayIdx) ) {//we want specific array element, but have entire array
                BacnetArrayVisitor visitor(mapping->propertyArrayIdx);
                propValue->_value->accept(&visitor);
                if (0 == visitor.dataGotten()) {//we got nothing - error
                    qDebug("%s : Couldn't read array index %d from property id %d. No error send back, since it's config problem.", __PRETTY_FUNCTION__, mapping->propertyArrayIdx, mapping->propertyId);
                    continue;
                } else
                    value = visitor.dataGotten();
            } else
                value = propValue->_value;

            if (!value.isNull()) {
                QVariant internalValue = value->toInternal();
                if (internalValue.isValid()) {
                    Q_CHECK_PTR(mapping->mappedProperty);
                    int result = mapping->mappedProperty->setValue(internalValue);
                    Q_ASSERT(Property::ResultOk == result);
                } else {
                    qDebug("%s : Cannot convert read property %d (array idx: %d) of object 0x%x to internal!",
                           __PRETTY_FUNCTION__, mapping->propertyId, mapping->propertyArrayIdx, mapping->objectId);
                    Q_ASSERT(false);
                }

            }
        }
    }

    Q_CHECK_PTR(covStrategy);
    covStrategy->notificationReceived(data, isConfirmed);
}
