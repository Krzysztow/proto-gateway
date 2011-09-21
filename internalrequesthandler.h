#ifndef ASYNCHRONOUSACTIONHANDLER_H
#define ASYNCHRONOUSACTIONHANDLER_H

#include <QList>

class BacnetObject;
class BacnetDeviceObject;
class InternalRequestHandler
{
public:
    InternalRequestHandler() {}
    virtual ~InternalRequestHandler() {}

//    virtual QList<int> execute() = 0;
    virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device) = 0;
    virtual bool isFinished() = 0;
    virtual void finalize(bool *deleteAfter) = 0;
    virtual bool execute() = 0;
};

#endif // ASYNCHRONOUSACTIONHANDLER_H
