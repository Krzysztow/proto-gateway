#ifndef INTERNALUNCONFIRMEDREQUESTHANDLER_H
#define INTERNALUNCONFIRMEDREQUESTHANDLER_H

#include "internalrequesthandler.h"
#include "bacnetaddress.h"

/** We stick to this upper class
  */

class BacnetUnconfirmedRequestData;

namespace Bacnet {
class BacnetTSM2;
class ExternalObjectsHandler;
class InternalObjectsHandler;

class InternalUnconfirmedRequestHandler:
        public InternalRequestHandler
{
public:
    InternalUnconfirmedRequestHandler();
    virtual ~InternalUnconfirmedRequestHandler();

public://overridden from InternalRequestHandler
    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device) = 0;
    virtual bool isFinished() = 0;
    virtual void finalize(bool *deleteAfter) = 0;
    virtual bool execute() = 0;
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length) = 0;
};

}
#endif // INTERNALUNCONFIRMEDREQUESTHANDLER_H
