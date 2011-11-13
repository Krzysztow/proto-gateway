#ifndef BACNETOBJECT_H
#define BACNETOBJECT_H

#include <QtCore>

#include "bacnetcommon.h"
#include "bacnetprimitivedata.h"
#include "covsupport.h"

struct WriteAccessSpecificationStruct;
class BacnetDataBase;
class BacnetDeviceObject;
class BacnetAddress;

namespace Bacnet{
    class Error;
    class ReadPropertyServiceData;
    class PropertyValue;
    class SubscribeCOVServiceData;
}
namespace BacnetProperty{enum Identifier;}

class Property;

class BacnetObject:
        public Bacnet::CovSupport
{
public:
    BacnetObject(Bacnet::ObjectIdentifier &id, BacnetDeviceObject *parentDevice);
    BacnetObject(BacnetObjectType::ObjectType objectType, quint32 instanceNumber, BacnetDeviceObject *parentDevice);
    virtual ~BacnetObject();


    //! Used to check if we can read from the device. If not yet, the asynchronous id for read request should be returned or error status.
    virtual int ensurePropertyReadyRead(BacnetProperty::Identifier propertyId) = 0;

    //! Returns the data associated with the propertyId.
    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(BacnetProperty::Identifier propId, quint32 arrayIdx, Bacnet::Error *error) = 0;
    virtual Bacnet::BacnetDataInterface *propertyReadInstantly(Bacnet::ReadPropertyServiceData *rpStruct, Bacnet::Error *error) = 0;

    virtual int ensurePropertyReadySet(Bacnet::PropertyValue &writeData, Bacnet::Error *error) = 0;

    Bacnet::ObjectIdentifier &objectId();
    quint32 objectIdNum();

    void setObjectName(QString name);
    QString objectName();

private:
    Bacnet::ObjectIdentifier _id;
    QString _name;

protected:
    BacnetDeviceObject *_parentDevice;
};

#endif // BACNETOBJECT_H
