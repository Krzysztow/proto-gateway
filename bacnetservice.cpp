#include "bacnetservice.h"

BacnetService::~BacnetService()
{
}

qint32 BacnetService::toRaw(quint8 *startPtr, quint16 buffLength)
{
    Q_UNUSED(startPtr);
    Q_UNUSED(buffLength);
    Q_ASSERT_X(false, "BacnetService::toRaw()", "Not owrwritten - correct it!");
    return -1;
}
