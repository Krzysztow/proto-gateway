#ifndef BACNETREADPROPERTYACK_H
#define BACNETREADPROPERTYACK_H

#include "bacnetservice.h"
#include "bacnetcommon.h"

namespace Bacnet {
    class BacnetDataInterface;
}

class BacnetReadPropertyAck:
        public BacnetService
{
public:
    BacnetReadPropertyAck();
    ~BacnetReadPropertyAck();

public:
    qint32 toRaw(quint8 *startPtr, quint16 buffLength);

    void setData(Bacnet::BacnetDataInterface *data, Bacnet::ReadPropertyStruct &ackReadPrptyData);

    //data that has been encoded in the ack
    Bacnet::ReadPropertyStruct _value;
    Bacnet::BacnetDataInterface *_data;
};

#endif // BACNETREADPROPERTYACK_H
