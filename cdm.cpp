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
        Q_ASSERT_X(false, "DataModel::createProperty()", "The property subject may be created only once!");
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

int DataModel::generateAsynchId()
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
        } /*else {//transaction relatively new
            _asynchIdStates[id].timeLeft -= _internalTimeout100ms;
        }*/
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
#include "internalobjectshandler.h"

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
#include "bacnetdeviceobject.h"
#include "externalobjectshandler.h"
#include "bacnettsm2.h"
#include "bacnetinternaladdresshelper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataModel *cdm = DataModel::instance();

    Bacnet::BacnetTSM2 *tsm = new BacnetTSM2();
    InternalObjectsHandler *bHndlr = new InternalObjectsHandler(tsm);

    QVariant test;
    test.setValue((double)72.3);

    BacnetAddress srcAddr;

    BacnetAddress destAddr;
    quint32 destAddrRaw(0x00000001);
    BacnetInternalAddressHelper::macAddressFromRaw((quint8*)&destAddrRaw, &destAddr);

    AsynchOwner *proto2 = new AsynchOwner();
    PropertySubject *subject = DataModel::instance()->createProperty(1, QVariant::Double);
    subject->setValue(test);
    proto2->addProperty(subject);

    BacnetDeviceObject *device = new BacnetDeviceObject(1);
    device->setObjectName("BacnetTestDevice");
    PropertyObserver *obs = DataModel::instance()->createPropertyObserver(1);
    device->addInternalProperty(BacnetProperty::PresentValue, obs);
    bHndlr->addDevice(BacnetInternalAddressHelper::internalAddress(destAddr), device);

    PropertySubject *subject2 = DataModel::instance()->createProperty(2, QVariant::Double);
    subject2->setValue(test);
    proto2->addProperty(subject2);

    PropertyObserver *obs2 = DataModel::instance()->createPropertyObserver(2);
    AnalogInputObject *aio = new AnalogInputObject(5, device);
    aio->setObjectName("HW_Setpoint");
    aio->addInternalProperty(BacnetProperty::PresentValue, obs2);

    BacnetDeviceObject *device1 = new BacnetDeviceObject(8);
    device1->setObjectName("BestDeviceEver");
    quint32 addr(0x00000003);
    BacnetAddress bAddr;
    BacnetInternalAddressHelper::macAddressFromRaw((quint8*)&addr, &bAddr);
    bHndlr->addDevice(BacnetInternalAddressHelper::internalAddress(bAddr), device1);

    AnalogInputObject *aio1 = new AnalogInputObject(3, device1);
    aio1->setObjectName("OATemp");

    ExternalObjectsHandler *extHandler = new ExternalObjectsHandler(tsm);
    bHndlr->_externalHandler = extHandler;
    PropertySubject *extSubject = DataModel::instance()->createProperty(3, QVariant::Double);
    extHandler->addMappedProperty(extSubject, BacnetObjectType::AnalogValue << 22 | 0x01,
                                  BacnetProperty::PresentValue, Bacnet::ArrayIndexNotPresent,
                                  0x00000001,
                                  BacnetExternalObjects::Access_ReadRequest);

    PropertyObserver *extObserver = DataModel::instance()->createPropertyObserver(3);
    proto2->addProperty(extObserver);

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
    bHndlr->getBytes(readPropertyService, sizeof(readPropertyService), srcAddr, destAddr);

//    //WRITE PROEPRTY ENCODED
//    quint8 wpService[] = {
//        0x00,
//        0x04,
//        0x59,
//        0x0F,

//        0x0c,
//        0x00, 0x00/*0x80*/, 0x00, 0x01,
//        0x19,
//        0x55,
//        0x3e,
//        0x44,
//        0x43, 0x34, 0x00, 0x00,
//        0x3f
//    };
//    bHndlr->getBytes(wpService, sizeof(wpService), srcAddr, destAddr);

//    //WHO IS
//    quint8 wiService[] = {
//        0x10,
//        0x08,
//        0x09, 0x03,
//        0x19, 0x03
//    };
//    bHndlr->getBytes(wiService, sizeof(wiService), srcAddr, destAddr);

//    //WHO HAS - object name is known
//    quint8 whoHasService[] = {
//        0x10,
//        0x07,
//        0x3d,
//        0x07,
//        0x00,
//        0x4F, 0x41, 0x54, 0x65, 0x6D, 0x70
//    };

//    BacnetAddress broadAddr;
//    broadAddr.setGlobalBroadcast();
//    bHndlr->getBytes(whoHasService, sizeof(whoHasService), srcAddr, broadAddr);

//    //WHO HAS - object id is known
//    quint8 whoHasService2[] = {
//        0x10,
//        0x07,
//        0x2c,
//        0x00, 0x00, 0x00, 0x03
//    };
//    bHndlr->getBytes(whoHasService2, sizeof(whoHasService2), srcAddr, broadAddr);



    return a.exec();
}
#endif //PCDM_TEST
