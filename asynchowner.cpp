#include "asynchowner.h"

#include "property.h"
#include "cdm.h"


AsynchOwner::AsynchOwner(QObject *parent):
        QObject(parent)
{
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));

   _timer->start(1000);
}

AsynchOwner::~AsynchOwner()
{
}

#include "cdm.h"
int AsynchOwner::getPropertyRequest(PropertySubject *toBeGotten)
{
    qint8 asynchId = DataModel::instance()->generateAsynchId();
    AsynchData reqData = {toBeGotten, asynchId, PropertyOwner::RequestGet, QVariant()};
    _asynchReqs.append(reqData);

    return asynchId;
}

void AsynchOwner::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    qDebug("This was invoked and action was %d!", asynchId);
}

void AsynchOwner::propertyValueChanged(Property *property)
{
    Q_UNUSED(property);
}

int AsynchOwner::setPropertyRequest(PropertySubject *toBeSet, QVariant &value)
{
    //here we can check, if the value is propert - for instance in allowable range
    Q_UNUSED(value);

    int asynchId = DataModel::instance()->generateAsynchId();
    AsynchData reqData = {toBeSet, asynchId, PropertyOwner::RequestSet, value};
    _asynchReqs.append(reqData);

    return asynchId;
}

void AsynchOwner::timeout() {
    if (_asynchReqs.isEmpty()) {
        qDebug("Asynch requests queue is empty!");
        return;
    }

    QVariant genValue;
    while (!_asynchReqs.isEmpty()) {
        AsynchData aData = _asynchReqs.takeFirst();

        if (PropertyOwner::RequestGet == aData.reqType) {
            if (aData.prop->type() == QVariant::Bool) {
                genValue.setValue((bool)(qrand()%2));
            } else if (aData.prop->type() == QVariant::Int) {
                genValue.setValue(qrand()%512 - 256);
            } else if (aData.prop->type() == QVariant::Double) {
                genValue.setValue(72.3);
            } else {
                Q_ASSERT_X(false, "cdm.cpp - test", "No other types checked here!");
            }
            aData.prop->setValueSilent(genValue);
            aData.prop->asynchActionFinished(aData.id, Property::ResultOk);
        } else if (PropertyOwner::RequestSet == aData.reqType) {
            aData.prop->asynchActionFinished(aData.id, Property::ResultOk);
        } else {
            Q_ASSERT_X(false, "cdm.cpp - test", "No other request is supported!");
        }
    }
}

void AsynchOwner::addProperty(Property *property)
{
    Q_ASSERT(property->owner() == 0);
    Q_CHECK_PTR(property);
    _properties.append(property);
    property->setOwner(this);

    if (dynamic_cast<PropertyObserver*>(property)) {
        tempProp = dynamic_cast<PropertyObserver*>(property);
        QTimer::singleShot(2000, this, SLOT(setExtValue()));
    }
}

void AsynchOwner::setExtValue()
{
    Q_CHECK_PTR(tempProp);
    QVariant value(180.0);
    qint16 ret = tempProp->setValue(value);
}
