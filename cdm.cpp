#include "cdm.h"

#include "property.h"
#include "propertyowner.h"

DataModel *DataModel::_instance = 0;
const int DataModel::UNUSED_TIME_VALUE = std::numeric_limits<int>::min();

DataModel::DataModel(QObject *parent):
        QObject(parent),
        _internalTimeout100ms(DEFAULT_TIMEOUT),
        _transactionTimer(new QTimer(this))
{
    initiateAsynchIds();

    connect(_transactionTimer, SIGNAL(timeout()), this, SLOT(internalTimeout()));
    _transactionTimer->start(100*_internalTimeout100ms);
}

DataModel::~DataModel()
{
    delete _transactionTimer;

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

void DataModel::initiateAsynchIds()
{
    _asynchIdStates.reserve(MAX_ASYNCH_ID);
    AsynchIdEntry nullEntry = {UNUSED_TIME_VALUE, 0, 0};
    for (int i = 0; i < MAX_ASYNCH_ID; ++i) {
        _asynchIdStates.append(nullEntry);
    }
}

int DataModel::getAsynchId()
{
    bool found = false;
    int id(0);
    for (; id < MAX_ASYNCH_ID; ++id) {
        if (isAsynchIdUnused(id)) {
            found = true;
            break;
        }
    }

    if (!found)
        return -1;

    _asynchIdStates[id].subjectProperty = 0;
    _asynchIdStates[id].requestingObserver = 0;
    _asynchIdStates[id].timeLeft = _internalTimeout100ms;

    ++id;
    return id;
}

void DataModel::setAsynchIdData(int asynchId, PropertySubject *subject, PropertyObserver *requester)
{
    --asynchId;
    Q_ASSERT(!isAsynchIdUnused(asynchId));
    _asynchIdStates[asynchId].subjectProperty = subject;
    _asynchIdStates[asynchId].requestingObserver = requester;
}

PropertySubject *DataModel::asynchActionSubject(int asynchId)
{
    --asynchId;
    Q_ASSERT(!isAsynchIdUnused(asynchId));
    if (isAsynchIdUnused(asynchId))
        return 0;
    else
        return _asynchIdStates[asynchId].subjectProperty;
}

PropertyObserver *DataModel::asynchActionRequester(int asynchId)
{
    --asynchId;
    Q_ASSERT(!isAsynchIdUnused(asynchId));
    if (isAsynchIdUnused(asynchId))
        return 0;
    else
        return _asynchIdStates[asynchId].requestingObserver;
}

void DataModel::releaseAsynchId(int id)
{
    --id;
    Q_ASSERT_X(id <= MAX_ASYNCH_ID, "DataModel::releaseAsynchId()", "Trying to release id, that is out of range!");
    if (id > MAX_ASYNCH_ID)
        return;

    //make sure this is used. However when events like timer timeout and frame received are used, it could happen
    //there will be two consecutive calls, which will make assertion fail.
    Q_ASSERT_X(!isAsynchIdUnused(id), "DataModel::releaseAsynchId()", "Attemp to release id that is already released!");

    setAsynchIdUnused(id);
}
#include <QDebug>
void DataModel::internalTimeout()
{
    qDebug()<<"Cleaning is started!";
    for (int id = 0; id<MAX_ASYNCH_ID; ++id) {
        if (!isAsynchIdUnused(id)) {
            if (_asynchIdStates[id].timeLeft <= _internalTimeout100ms) {
                if (_asynchIdStates[id].timeLeft < 0) {//the time this transaction is outstanding is within [_internalTimeout100ms, 2*_internalTimeout100ms). Time to clean!
                    if (_asynchIdStates[id].subjectProperty != 0) {
                        _asynchIdStates[id].subjectProperty;//timeout INTERNAL_TIMEOUT
                    }
                    if (_asynchIdStates[id].requestingObserver != 0) {
                        _asynchIdStates[id].requestingObserver;//timeout INTERNAL_TIMEO
                    }
                    setAsynchIdUnused(id);
                } else {//clean it next time
                    _asynchIdStates[id].timeLeft = -1;//mark it to be cleaned next time
                }
            }
        } else {//transaction relatively new
            _asynchIdStates[id].timeLeft -= _internalTimeout100ms;
        }
    }
}

//TEST

#define CDM_TEST
#ifdef CDM_TEST
#include <QDebug>
#include <QtCore>

#include "propertyowner.h"
#include "asynchowner.h"
#include "asynchsetter.h"

#include <QCoreApplication>
#include <QObject>
#include <private/qobject_p.h>

#include <sys/time.h>

#include "bacnetcommon.h"

struct KElement {
    int timeLeft;
    PropertyOwner *owner;
    PropertyOwner *requester;
};

//class TestObject
//{
//public:
//    TestObject(Bacnet::ObjectId identifier):
//    _id(identifier)
//    {}

//    TestObject(quint32 instance, Bacnet::ObjectType type)
//    {
//        _id.instanceNum = instance;
//        _id.objectType = type;
//    }

//    Property *getPropertyPtr(BacnetProperty::BacnetPropertyIdentifier propId) {
//        Property *retProp = _objectProperties.value(propId);
//        if (0 == retProp) {
//            retProp = BacnetDefaultObject::instance()->getProperty(_id.objectType, propId);
//        }
//        return retProp;
//    }

//    void addProperty(BacnetProperty::BacnetPropertyIdentifier propId, Property *property) {
//        Property *oldProp = _objectProperties.value(propId);
//        _objectProperties.insert(propId, property);
//        Q_ASSERT(0 != oldProp);//there was other property
//        if (0 != oldProp) {
//            qDebug("TestObject: addProperty() - there was already a property with such id.");
//            delete oldProp;
//        }
//    }

//    quint32 objId() {
//        return ((_id.objectType & 0x03ff) << 22) |
//                (_id.instanceNum & 0x3fffff);
//    }

//private:
//    Bacnet::ObjectId _id;
//    QMap<BacnetProperty::BacnetPropertyIdentifier, Property*> _objectProperties;
//};

//class DeviceObject
//{
//public:
//    DeviceObject(quint32 instance):
//            _object(instance, Bacnet::ObjectTypeDevice)
//    {}

//private:
//    TestObject _object;
//};

#include "helpercoder.h"
#include "bacnettagparser.h"

#include "helpercoder.h"
#include "bacnetprimitivedata.h"

int main(int argc, char *argv[])
{
    int i = 4;
    qDebug()<<"RESULT"<<i%8;
    return 0;

    QCoreApplication a(argc, argv);

    DataModel *cdm = DataModel::instance();

    AsynchSetter *proto1 = new AsynchSetter();

    QVariant test;
    test.setValue((float)0.4999999);

    AsynchOwner *proto2 = new AsynchOwner();

    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Bool);
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);

    proto2->addProperty(subject);
    proto1->addProperty(obs, 111);

    subject = DataModel::instance()->createProperty(2, QVariant::Int);
    proto2->addProperty(subject);
    proto1->addProperty(DataModel::instance()->createPropertyObserver(2), 112);
    proto1->addProperty(DataModel::instance()->createPropertyObserver(2), 110);

//    proto1->exec();

    //READ PROPERTY ENCODED

    quint8 readPropertyService[] = {
        0x00,
        0x00,
        0x01,
        0x0C,
        0x0C,
        0x00, 0x00, 0x00, 0x05,
        0x19,
        0x55
    };



    qDebug()<<"Sizeof QObject "<<sizeof(QObject)<<sizeof(QObjectPrivate)<<sizeof(QObjectData);


    return a.exec();
}
#endif //PCDM_TEST
