#ifndef BacnetErrorAck_H
#define BacnetErrorAck_H

#include "bacnetservice.h"
#include "bacnetcommon.h"

class BacnetErrorAck:
        public BacnetService
{
public:
    BacnetErrorAck(Bacnet::ErrorStruct error);
    ~BacnetErrorAck();

private:
    Bacnet::ErrorStruct _error;
};

#endif // BacnetErrorAck_H
