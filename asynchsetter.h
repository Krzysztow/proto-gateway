#ifndef ASYNCHSETTER_H
#define ASYNCHSETTER_H

#include "propertyowner.h"
#include "bacnetcommon.h"

class BacnetDeviceObject;
class BacnetObject;
class BacnetService;
class Property;
class InternalRequestHandler;

namespace Bacnet {class BacnetTSM2;}

class AsynchSetter
{
public:
    AsynchSetter(Bacnet::BacnetTSM2 *tsm);

public://interface for BacnetObject-Internal interaction
    void propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
    int propertyRequested(BacnetDeviceObject *device, BacnetObject *object, BacnetProperty::Identifier propId);

public:
    bool addDevice(int address, BacnetDeviceObject *device);

    void getBytes(quint8 *data, quint16 length);

public:
    QMap<quint32, BacnetDeviceObject*> _devices;
    QHash<int, InternalRequestHandler*> _asynchRequests;

    Bacnet::BacnetTSM2 *_tsm;
};


#endif // ASYNCHSETTER_H
