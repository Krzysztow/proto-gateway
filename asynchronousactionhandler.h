#ifndef ASYNCHRONOUSACTIONHANDLER_H
#define ASYNCHRONOUSACTIONHANDLER_H

#include <QList>

class BacnetObject;
class BacnetDeviceObject;
class AsynchronousActionHandler
{
public:
    AsynchronousActionHandler() {}
    virtual ~AsynchronousActionHandler() {}

    virtual QList<int> execute() = 0;
    virtual bool asynchActionFinished(int asynchId, int result, BacnetObject *object, BacnetDeviceObject *device) = 0;
    virtual bool isFinished() = 0;
    virtual void finalize(bool &deleteAfter) = 0;
};

#endif // ASYNCHRONOUSACTIONHANDLER_H
