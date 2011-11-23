#ifndef ASYNCHRONOUSACTIONHANDLER_H
#define ASYNCHRONOUSACTIONHANDLER_H

#include <QList>

namespace Bacnet {
    class BacnetObject;
    class BacnetDeviceObject;
}

class InternalRequestHandler
{
public:
    InternalRequestHandler() {}
    virtual ~InternalRequestHandler() {}

    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device) = 0;
    virtual bool isFinished() = 0;
    virtual void finalize(bool *deleteAfter) = 0;
    virtual bool execute() = 0;

    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length) = 0;
};

#endif // ASYNCHRONOUSACTIONHANDLER_H
