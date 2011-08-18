#include "cdm.h"

DataModel *DataModel::_instance = 0;

DataModel::DataModel()
{
}

DataModel *DataModel::instance()
{
    if (0 == _instance)
        _instance = new DataModel();

    return _instance;
}

PropertySubject *DataModel::createProperty(quint32 propId, QVariant::Type propertyType)
{
    if (_properties.contains(propId)) {
        return 0;
    }

    PropertySubject *propS = new PropertySubject(0, propertyType);
    _properties.insert(propId, propS);

    return propS;
}

PropertySubject *DataModel::getProperty(quint32 propId)
{
    return _properties.value(propId);
}

PropertyObserver *DataModel::createPropertyObserver(quint32 propId)
{
    PropertySubject *propS = _properties.value(propId);
    if (0 == propS)
        return 0;

    PropertyObserver *propO = new PropertyObserver(0, propS);
    return propO;
}

//TEST

#define CDM_TEST
#ifdef CDM_TEST
#include <QDebug>

#include "propertyowner.h"
#include "asynchowner.h"
#include "asynchsetter.h"

#include <QCoreApplication>
#include <QObject>
#include <private/qobject_p.h>

int main(int argc, char *argv[])
{
    QVariant val(QVariant::Bool);

    val.setValue((bool)1);
    val.clear();
    val.setValue(false);

    QCoreApplication a(argc, argv);

    DataModel *cdm = DataModel::instance();

    AsynchSetter *proto1 = new AsynchSetter();

    QVariant test;
    test.setValue((float)0.4999999);

    qDebug()<<"WHAT I GET HERE IS"<<test.toBool();

    AsynchOwner *proto2 = new AsynchOwner();

    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Bool);
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);

    proto2->addProperty(subject);
    proto1->addProperty(obs, 111);

    subject = DataModel::instance()->createProperty(2, QVariant::Int);
    proto2->addProperty(subject);
    proto1->addProperty(DataModel::instance()->createPropertyObserver(2), 112);
    proto1->addProperty(DataModel::instance()->createPropertyObserver(2), 110);

    proto1->exec();

    qDebug()<<"Sizeof QObject "<<sizeof(QObject)<<sizeof(QObjectPrivate)<<sizeof(QObjectData);


    return a.exec();
}
#endif //PCDM_TEST
