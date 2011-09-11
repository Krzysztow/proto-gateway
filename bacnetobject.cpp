#include "bacnetobject.h"

#include "property.h"
#include "bacnetdeviceobject.h"

BacnetObject::BacnetObject(Bacnet::ObjectIdStruct id):
        _id(id)
{
}

BacnetObject::BacnetObject(BacnetObjectType::ObjectType objectType, quint32 instanceNumber)
{
    _id.objectType = objectType;
    _id.instanceNum = instanceNumber;
}

BacnetObject::~BacnetObject()
{
}

Bacnet::ObjectIdStruct &BacnetObject::objectId()
{
    return _id;
}

quint32 BacnetObject::objectIdNum()
{
    return (_id.objectType << 22) | (_id.instanceNum);
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
