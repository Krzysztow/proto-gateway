#include "bacnetcommon.h"

quint32 Bacnet::objIdToNum(const Bacnet::ObjectIdStruct &objId) {
    return ( (objId.objectType << 22) | (0x3fffff & objId.instanceNum) );
}

Bacnet::ObjectIdStruct Bacnet::numToObjId(unsigned int numObjId) {
    Bacnet::ObjectIdStruct objIdStr = {
        (BacnetObjectType::ObjectType)(numObjId >> 22),
        numObjId & 0x3fffff
    };

    return objIdStr;
}

uint Bacnet::qHash(ObjectIdStruct objId)
{
    return objIdToNum(objId);
}

bool Bacnet::operator==(ObjectIdStruct one, ObjectIdStruct other)
{
    return objIdToNum(one) == objIdToNum(other);
}
