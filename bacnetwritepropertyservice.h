#ifndef BACNETWRITEPROPERTYSERVICE_H
#define BACNETWRITEPROPERTYSERVICE_H

#include <QtCore>

#include "bacnetservice.h"
#include "bacnetcommon.h"
#include "error.h"

class BacnetDeviceObject;
class BacnetWritePropertyService:
        public BacnetService
{
public:
    BacnetWritePropertyService();
    BacnetWritePropertyService(Bacnet::ObjectIdStruct objId, BacnetProperty::Identifier propertyId,
                              Bacnet::BacnetDataInterface *writeValue, quint32 arrayIndex = Bacnet::ArrayIndexNotPresent);

    ~BacnetWritePropertyService();


    qint16 fromRaw(quint8 *servicePtr, quint16 length);
    qint32 toRaw(quint8 *startPtr, quint8 buffLength);

public:
    virtual qint32 execute(BacnetDeviceObject *device);
    virtual BacnetService *takeResponse();
    virtual bool hasError();
    virtual Bacnet::Error &error();

    QList<int> asynchIds();
    bool asynchActionFinished(int asynchId, int result, BacnetDeviceObject *device, BacnetObject *object);
    bool isReady();

private:
    bool finishWriting_helper(BacnetDeviceObject *object, int resultCode = 0);

private:
    Bacnet::ObjectIdStruct _objectId;
    Bacnet::PropertyValueStruct _propValue;

    BacnetService *_response;
    Bacnet::Error _error;
    quint32 _asynchId;
};

#endif // BACNETWRITEPROPERTYSERVICE_H
