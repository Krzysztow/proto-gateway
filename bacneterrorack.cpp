#include "bacneterrorack.h"

BacnetErrorAck::BacnetErrorAck(Bacnet::Error error):
        _error(error)
{
}

BacnetErrorAck::~BacnetErrorAck()
{
}
