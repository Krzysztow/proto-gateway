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
#ifndef CDM_TEST
//int main()
//{
//    return 0;
//}
#else
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


#include "helpercoder.h"
#include "bacnettagparser.h"

#include "helpercoder.h"
#include "bacnetprimitivedata.h"
#include "bacnetpci.h"
#include "bacnetreadpropertyservice.h"
#include "analoginputobject.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataModel *cdm = DataModel::instance();

    AsynchSetter *proto1 = new AsynchSetter();

    QVariant test;
    test.setValue((double)72.3);

    AsynchOwner *proto2 = new AsynchOwner();

    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Double);
    subject->setValue(test);
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);

    proto2->addProperty(subject);
    proto1->addProperty(obs, BacnetProperty::PresentValue);

    subject = DataModel::instance()->createProperty(2, QVariant::Int);
    proto2->addProperty(subject);
    proto1->addProperty(DataModel::instance()->createPropertyObserver(2), 112);
    proto1->addProperty(DataModel::instance()->createPropertyObserver(2), 110);


    PropertySubject *subject2 = DataModel::instance()->createProperty(3, QVariant::Double);
    subject2->setValue(test);
    proto2->addProperty(subject2);
    PropertyObserver *obs2 = DataModel::instance()->createPropertyObserver(3);
    obs2->setOwner(proto1);
    BacnetObject *aio = proto1->_device->bacnetObject(BacnetObjectType::AnalogInput << 22 | 1);
    ((AnalogInputObject*)aio)->_cdmProperties.insert(BacnetProperty::PresentValue, obs2);


    //READ PROPERTY ENCODED

//    quint8 readPropertyService[] = {
//        0x00,
//        0x00,
//        0x01,
//        0x0C,
//        0x0C,
//        0x00, 0x00, 0x00, 0x05,
//        0x19,
//        0x55
//    };
//    proto1->getBytes(readPropertyService, sizeof readPropertyService);


    quint8 wpService[] = {
        0x00,
        0x04,
        0x59,
        0x0F,

        0x0c,
        0x00, 0x00/*0x80*/, 0x00, 0x01,
        0x19,
        0x55,
        0x3e,
        0x44,
        0x43, 0x34, 0x00, 0x00,
        0x3f
    };
    proto1->getBytes(wpService, sizeof(wpService));

    return a.exec();

//    KAddress *address = new KAddress(8);
//    quint8 data[] = {0x00, 0x04, 0x59, 0x0F, 0x0C,
//                    0x00, 0x80, 0x00, 0x01,
//                    0x19, 0x55, 0x3E,
//                    0x44,
//                    0x43, 0x34, 0x00, 0x00,
//                    0x3F};
//    quint8 *dataPtr = data;
//    quint16 length = sizeof(data);

//    qint16 ret(0);
//    BacnetPciData *pciData = BacnetPci::createPciData(dataPtr, length, &ret);
//    if (ret <= 0) {
//        Q_ASSERT(false);//some error
//        //send reject/abort message
//    }
//    dataPtr += ret;
//    length -= ret;


//    BacnetService *service = ServiceFactory::createService(dataPtr, length, pciData->pduType(), &ret);
//    if (ret <= 0) {
//        Q_ASSERT(false);//some error
//        //send reject/abort message
//    }
}
#endif //PCDM_TEST
