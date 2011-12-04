#include "property.h"
#include "propertyobserver.h"
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
