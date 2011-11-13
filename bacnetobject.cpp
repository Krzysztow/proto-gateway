#include "bacnetobject.h"

#include "property.h"
#include "bacnetdeviceobject.h"
#include "bacnetaddress.h"
#include "subscribecovservicedata.h"
#include "error.h"

BacnetObject::BacnetObject(Bacnet::ObjectIdentifier &id, BacnetDeviceObject *parentDevice):
    _id(id),
    _parentDevice(parentDevice)
{
}

BacnetObject::BacnetObject(BacnetObjectType::ObjectType objectType, quint32 instanceNumber, BacnetDeviceObject *parentDevice):
    _id(objectType, instanceNumber),
    _parentDevice(parentDevice)
{
}

BacnetObject::~BacnetObject()
{
}

Bacnet::ObjectIdentifier &BacnetObject::objectId()
{
    return _id;
}

quint32 BacnetObject::objectIdNum()
{
    return _id.objectIdNum();
}

void BacnetObject::setObjectName(QString name)
{
    _name = name;
}

QString BacnetObject::objectName()
{
    //! \todo If there are performance problems/fragmentation issues - let's make it static QString - good if objectName is not kept for
    //! long time. Otherwise another instance call would change the value.
    if (_name.isEmpty())
        return QString("SNGBacnet:%1").arg(objectIdNum());
    return _name;
}

//QList<Bacnet::PropertyValue*> BacnetObject::readCovValuesList()
//{
//    return QList<Bacnet::PropertyValue*>();
//}


