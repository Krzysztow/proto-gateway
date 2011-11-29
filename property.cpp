#include "property.h"
#include "propertyowner.h"

#include "cdm.h"

///////////////Property

Property::Property()
{
}

Property::~Property()
{
}

int Property::generateAsynchId()
{
    int id = DataModel::instance()->generateAsynchId();
    Q_ASSERT(id >= 0);
    return id;
}

void Property::releaseId(int asynchId)
{
    DataModel::instance()->releaseAsynchId(asynchId);
}


void Property::setOwner(PropertyOwner *owner)
{
    Q_UNUSED(owner);
}

PropertyOwner *Property::owner()
{
    return 0;
}

///////////////PropertySubject

PropertySubject::PropertySubject(PropertyOwner *container, QVariant::Type propType):
        _owner(container)
{
    _value.setValue(QVariant(propType));
}

int PropertySubject::setValue(QVariant &inValue)
{
    if (inValue.type() != _value.type()) {
        Q_ASSERT(inValue.type() == _value.type());
        return Property::TypeMismatch;
    }

    setValueInstantly(inValue, 0);
    return Property::ResultOk;
}
#warning "Don't compare types - check if one can convert one to the other!"
int PropertySubject::getValue(QVariant *outValue)
{
    if (0 != outValue) {
        if ( !outValue->isNull() && (outValue->type() != _value.type()) ) { //if the QVariant has type, be sure they are the same
            Q_ASSERT(outValue->isNull() || (outValue->type() == _value.type()));
            return TypeMismatch;
        }

        *outValue = _value;
    }
    return Property::ResultOk;
}

int PropertySubject::getValueInstant(QVariant *outValue)
{
    Q_ASSERT(0 != outValue);//be sure pointer is not null
    if (0 != outValue) {
        if ( !outValue->isNull() && (outValue->type() != _value.type()) ) { //if the QVariant has type, be sure they are the same
            Q_ASSERT(outValue->isNull() || (outValue->type() == _value.type()));
            return TypeMismatch;
        }
        *outValue = _value;
    }
    return Property::ResultOk;
}

void PropertySubject::setOwner(PropertyOwner *owner)
{
    _owner = owner;
}

PropertyOwner *PropertySubject::owner()
{
    return _owner;
}

QVariant::Type PropertySubject::type()
{
    return _value.type();
}

int PropertySubject::getValueSafe(QVariant *outValue, PropertyObserver *requester)
{
    if (0 != outValue) {
        if ( !outValue->isNull() && (outValue->type() != _value.type()) ) { //if the QVariant has type, be sure they are the same
            Q_ASSERT(outValue->isNull() || (outValue->type() == _value.type()));
            return TypeMismatch;
        }
    }

    int ret(Property::ResultOk);
    if (0 != _owner) {
        ret = _owner->getPropertyRequested(this);
    }

    if (Property::ResultOk == ret) {
        if (0 != outValue)
            *outValue = _value;
    } else if (ret >= 0) {
        DataModel::instance()->setAsynchIdData(ret, this, requester);
    }

    return ret;
}

int PropertySubject::setValueSafe(QVariant &inValue, PropertyObserver *requester)
{
    if (!inValue.canConvert(_value.type())) {
        Q_ASSERT(inValue.type() == _value.type());
        return Property::TypeMismatch;
    }

    int ret(Property::ResultOk);
    if (0 != _owner)
        ret = _owner->setPropertyRequested(this, inValue);

    if (Property::ResultOk == ret) {
        if (_value != inValue) {//set and inform only if there is a difference in values.
            setValueInstantly(inValue, requester);
        }
    } else if (ret >= 0) {
        DataModel::instance()->setAsynchIdData(ret, this, requester);
    }

    return ret;
}

//void PropertySubject::asynchSetValueResponse(int asynchId, QVariant &value, bool success, Property *originRequester)
//{
//    // TODO (are we sure/should we be sure) that the value passed is the same as _value? Types should be the same.
//    Q_ASSERT(value.type() == _value.type());
//    int index = _observers.indexOf((PropertyObserver*)originRequester);//not nice casting, however we are sure there are only PropertyObservers instances - we compare pointers
//    if (index >= 0) {//this was one of my observers requesting
//        _observers.at(index)->asynchSetValueResponse(asynchId, value, success, 0);
//    } else {
//        foreach (PropertyObserver *observer, _observers)
//            observer->asynchSetValueResponse(asynchId, value, success, 0);
//    }
//}

//void PropertySubject::asynchGetValueResponse(int asynchId, QVariant &value, bool success, Property *originRequester)
//{
//    Q_ASSERT(value.type() == _value.type());
//    int index = _observers.indexOf((PropertyObserver*)originRequester);//we are looking for a pointer match, so don't care about nasty casting.
//    if (index >= 0) {//this was one of my observers requesting
//        _observers.at(index)->asynchSetValueResponse(asynchId, value, success, 0);
//    } else {
//        foreach (PropertyObserver *observer, _observers)
//            observer->asynchSetValueResponse(asynchId, value, success, 0);
//    }
//}

void PropertySubject::asynchActionFinished(int asynchId, Property::ActiontResult actionResult)
{
    Q_ASSERT(this == DataModel::instance()->asynchActionSubject(asynchId));
    PropertyObserver *requester = DataModel::instance()->asynchActionRequester(asynchId);
    Q_CHECK_PTR(requester);

    int index = _observers.indexOf(requester);
    if (index >= 0) {//this was one of my observers requesting, so tell him we are done
        PropertyObserver *obs = _observers.at(index);
        obs->asynchActionFinished(asynchId, actionResult);
//        _observers.at(index)->asynchActionFinished(asynchId, actionResult);
    }

    //if the value is changed with success - tell other observers that it has changed
    if (Property::ResultOk == actionResult) {
        foreach (PropertyObserver *observer, _observers) {
            if (observer != requester)
                observer->propertyValueChanged();
        }
    } else
        qDebug("PropertySubject::asynchActionFinished() - result %d, don't tell others!", actionResult);

    DataModel::instance()->releaseAsynchId(asynchId);
}

void PropertySubject::setValueInstantly(QVariant &inValue, PropertyObserver *observerToOmit)
{
    Q_ASSERT(_value.type() == inValue.type());
    _value = inValue;
    foreach (PropertyObserver *observer, _observers) {
        if (observer != observerToOmit)
                observer->propertyValueChanged();
    }
}

void PropertySubject::setValueSilent(QVariant &inValue)
{
    Q_ASSERT(inValue.canConvert(_value.type()));
    if (inValue.convert(_value.type())) {
        _value = inValue;
    }
    else
        qWarning("Couldn't convert value (%d to %d)!", inValue.type(), _value.type());
}

void PropertySubject::addObserver(PropertyObserver *observer)
{
    Q_ASSERT(observer);
    if (!_observers.contains(observer))
        _observers.append(observer);
}

void PropertySubject::rmObserver(PropertyObserver *observer)
{
    Q_ASSERT(observer);
    _observers.removeOne(observer);
}

///////////////PropertObserver

PropertyObserver::PropertyObserver(PropertyOwner *container, PropertySubject *property):
        _property(property),
        _owner(container)
{
    Q_ASSERT(_property);
    _property->addObserver(this);
}

PropertyObserver::~PropertyObserver()
{
    _property->rmObserver(this);
}

void PropertyObserver::setOwner(PropertyOwner *owner)
{
    _owner = owner;
}

PropertyOwner *PropertyObserver::owner()
{
    return _owner;
}

int PropertyObserver::getValue(QVariant *outValue)
{
    return _property->getValueSafe(outValue, this);
}

QVariant::Type PropertyObserver::type()
{
    return _property->type();
}

int PropertyObserver::setValue(QVariant &inValue)
{
    return _property->setValueSafe(inValue, this);
}

void PropertyObserver::propertyValueChanged()
{
    _owner->propertyValueChanged(this);
}

int PropertyObserver::getValueInstant(QVariant *outValue)
{
    Q_ASSERT(_property);
    //we know it's not Property, but PropertySubject, so no virtual function is called, but straight PropertySubject one.
    return _property->getValue(outValue);
}


//void PropertyObserver::asynchSetValueResponse(int asynchId, QVariant &value, bool success, Property *originRequester)
//{
//    Q_UNUSED(asynchId);
//    Q_UNUSED(value);
//    Q_UNUSED(success);
//    Q_UNUSED(originRequester);
//    if (0 != _owner)
//        _owner->setRequestResult(asynchId, success);
//}

//void PropertyObserver::asynchGetValueResponse(int asynchId, QVariant &value, bool success, Property *originRequester)
//{
//    Q_UNUSED(asynchId);
//    Q_UNUSED(value);
//    Q_UNUSED(success);
//    Q_UNUSED(originRequester);
//    if (0 != _owner)
//        _owner->getRequestResult(asynchId, success);
//}

void PropertyObserver::asynchActionFinished(int asynchId, Property::ActiontResult actionResult)
{
    if (0 != _owner)
        _owner->asynchActionFinished(asynchId, this, actionResult);
}

///////////////PROPERTY UNSHARED
PropertyUnshared::PropertyUnshared(QVariant::Type propType):
        _value(propType)
{
}

int PropertyUnshared::getValueSafe(QVariant *outValue)
{
    if (0 != outValue)
        *outValue = _value;
    return Property::ResultOk;
}

int PropertyUnshared::setValueSafe(QVariant &inValue)
{
    //! \todo Remove code duplication (same as in PropertySubject::setValue())
    if (inValue.type() != _value.type()) {
        Q_ASSERT(inValue.type() == _value.type());
        return Property::TypeMismatch;
    }

    _value = inValue;
    return Property::ResultOk;
}

QVariant::Type PropertyUnshared::type()
{
    return _value.type();
}

//TEST

//#define PROPERTY_TEST
#ifdef PROPERTY_TEST
int main()
{
    qDebug()<<"sizeof "<<sizeof(Property)<<sizeof(PropertyContainer)<<sizeof(PropertyObserver)<<
            sizeof(PropertyUnshared)<<sizeof(PropertyUnshared);


    QVariant var;
    PropertyContainer pContainer;

    PropertySubject pSubject(&pContainer, QVariant::Bool);

    PropertyContainer pContainer2;
    PropertyObserver pObserver1(&pContainer2, &pSubject);
    PropertyObserver pObserver2(&pContainer2, &pSubject);

    PropertyContainer pContainer3;
    PropertyObserver pObserver3(&pContainer3, &pSubject);
    Property *pr = new PropertyObserver(&pContainer, &pSubject);

    var.setValue(true);
    qDebug()<<"Setting to int"<<pSubject.setValue(var);
    var.clear();
    Q_ASSERT(pObserver3.type() == QVariant::Bool);

    delete pr;

    var.setValue(false);
    qDebug()<<"Setting to int"<<pSubject.setValue(var);


    qDebug()<<"Property type"<<pObserver3.getValue(&var);
    qDebug()<<"Value gotten"<<var;



    return 0;
}
#endif //PROPERTY_TEST
