#ifndef ASYNCHSETTER_H
#define ASYNCHSETTER_H

#include "propertyowner.h"
#include "bacnetcommon.h"
#include "bacnetaddress.h"
#include "bacnetinternaladdresshelper.h"
#include "externalobjectshandler.h"

class BacnetDeviceObject;
class BacnetObject;
class BacnetService;
class Property;
class InternalRequestHandler;


namespace Bacnet {class BacnetTSM2;}

class InternalObjectsHandler
{
public:
    InternalObjectsHandler(Bacnet::BacnetTSM2 *tsm);

public://interface for BacnetObject-Internal interaction
    void propertyIoFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device);
    void addAsynchronousHandler(QList<int> asynchIds, InternalRequestHandler *handler);

public:
    bool addDevice(InternalAddress address, BacnetDeviceObject *device);
    QMap<quint32, BacnetDeviceObject*> &virtualDevices();

    //! \todo If performance here is bad, just return reference to QMap, as is stored.
    QList<BacnetDeviceObject*> devices();

    void getBytes(quint8 *data, quint16 length, BacnetAddress &srcAddress, BacnetAddress &destAddress);

#warning "Remove this section!"
public:
    Bacnet::ExternalObjectsHandler *_externalHandler;

public:
    QMap<InternalAddress, BacnetDeviceObject*> _devices;
    QHash<int, InternalRequestHandler*> _asynchRequests;

    Bacnet::BacnetTSM2 *_tsm;
};


#endif // ASYNCHSETTER_H
