#include "bacneterrorack.h"

BacnetErrorAck::    BacnetErrorAck(Bacnet::ErrorStruct error):
        _error(error)
{
}

BacnetErrorAck::~BacnetErrorAck()
{
}
