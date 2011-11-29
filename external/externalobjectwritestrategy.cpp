#include "externalobjectwritestrategy.h"

#include "property.h"
#include "externalpropertymapping.h"
#include "writepropertyservicedata.h"
#include "externalobjectshandler.h"
#include "bacnetdefaultobject.h"
#include "bacnetwritepropertyservicehandler.h"

using namespace Bacnet;

int Bacnet::ExternalObjectWriteStrategy::writeProperty(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler, QVariant &valueToWrite, bool generateAsynchId)
{
    Q_CHECK_PTR(propertyMapping);
    Q_CHECK_PTR(propertyMapping->mappedProperty);

    int asynchId(Property::ResultOk);
    if (generateAsynchId)
        asynchId = propertyMapping->mappedProperty->generateAsynchId();
    Q_ASSERT(asynchId >= 0);
    if (asynchId < 0) {
        qWarning("Can't generate asynchronous id.");
        return Property::UnknownError;
    }

    ObjectIdentifier objectId(propertyMapping->objectId);
    BacnetDataInterface *writeData = BacnetDefaultObject::createDataForObjectProperty(objectId.type(), propertyMapping->propertyId, propertyMapping->propertyArrayIdx);
    Q_CHECK_PTR(writeData);
    if (0 == writeData) {
        qWarning("Can't create appropriate value isntance for %d, %d", objectId.type(), propertyMapping->propertyId);
        if (asynchId > 0)
            propertyMapping->mappedProperty->releaseId(asynchId);
        return Property::UnknownError;
    }

    if (!writeData->setInternal(valueToWrite)) {
        qDebug("ExternalObjectsHandler::setPropertyRequest() : Can't convert variant type %d, to bacnet type %d",
               valueToWrite.type(), writeData->typeId());
        delete writeData;
        if (asynchId > 0)
            propertyMapping->mappedProperty->releaseId(asynchId);
        return Property::TypeMismatch;
    }

    WritePropertyServiceData *serviceData =
            new WritePropertyServiceData(objectId, propertyMapping->propertyId, writeData, propertyMapping->propertyArrayIdx);
    Q_CHECK_PTR(serviceData);
    BacnetWritePropertyServiceHandler *serviceHandler =
            new BacnetWritePropertyServiceHandler(serviceData, asynchId, propertyMapping);
    Q_CHECK_PTR(serviceHandler);

    //the ownership isgiven to AppLayer. We just use pointers as Asynchronous tokens.
    externalHandler->send(serviceHandler, propertyMapping->objectId);
    return asynchId;
}

bool ExternalObjectWriteStrategy::isPeriodic()
{
    return false;
}

bool ExternalObjectWriteStrategy::timePassed(int timePassed_ms)
{
    Q_UNUSED(timePassed_ms);
    qDebug("%s : Don't call me", __PRETTY_FUNCTION__);
    Q_ASSERT(false);
    return false;
}

void ExternalObjectWriteStrategy::doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler)
{
    Q_UNUSED(propertyMapping);
    Q_UNUSED(externalHandler);
    qDebug("%s : Don't call me", __PRETTY_FUNCTION__);
    Q_ASSERT(false);
}

void ExternalObjectWriteStrategy::actionFinished(ExternalObjectWriteStrategy::FinishStatus finishStatus)
{
    Q_UNUSED(finishStatus);
}
