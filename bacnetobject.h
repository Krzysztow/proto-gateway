#ifndef BACNETOBJECT_H
#define BACNETOBJECT_H

#include <QtCore>

#include "bacnetcommon.h"
#include "bacnetprimitivedata.h"

struct WriteAccessSpecificationStruct;
class BacnetDataBase;
class BacnetDeviceObject;
namespace Bacnet{
    class Error;
    class ReadPropertyServiceData;
    class PropertyValue;
}
namespace BacnetProperty{enum Identifier;}

class Property;

class BacnetObject
{
public:
    BacnetObject(Bacnet::ObjectIdStruct id);
    BacnetObject(BacnetObjectType::ObjectType objectType, quint32 instanceNumber);
    virtual ~BacnetObject();


    //! Used to check if we can read from the device. If not yet, the asynchronous id for read request should be returned or error status.
    virtual int ensurePropertyReadyRead(BacnetProperty::Identifier propertyId) = 0;

    //! Returns the data associated with the propertyId.
    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(BacnetProperty::Identifier propId, quint32 arrayIdx, Bacnet::Error *error) = 0;
    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(Bacnet::ReadPropertyServiceData *rpStruct, Bacnet::Error *error) = 0;

    virtual int ensurePropertyReadySet(Bacnet::PropertyValueStruct &writeData, Bacnet::Error *error) = 0;

    //so far this is not compulsory. Function returns 0 on default, if not supported.
    virtual QList<Bacnet::PropertyValue*> readCovValuesList();

    Bacnet::ObjectIdStruct &objectId();
    quint32 objectIdNum();

    void setObjectName(QString name);
    QString objectName();

//public://overridden from PropertyOwner
//    virtual int getPropertyRequest(PropertySubject *toBeGotten) = 0;
//    virtual int setPropertyRequest(PropertySubject *toBeSet, QVariant &value) = 0;
//    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult) = 0;

private:
    Bacnet::ObjectIdStruct _id;
    QString _name;
};

#endif // BACNETOBJECT_H
