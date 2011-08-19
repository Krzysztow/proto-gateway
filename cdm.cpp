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

#include "propertyowner.h"
#include "asynchowner.h"
#include "asynchsetter.h"

#include <QCoreApplication>
#include <QObject>
#include <private/qobject_p.h>

#include <sys/time.h>

struct KElement {
    int timeLeft;
    PropertyOwner *owner;
    PropertyOwner *requester;
};

int main(int argc, char *argv[])
{
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

    proto1->exec();

    qDebug()<<"Sizeof QObject "<<sizeof(QObject)<<sizeof(QObjectPrivate)<<sizeof(QObjectData);


    return a.exec();
}
#endif //PCDM_TEST