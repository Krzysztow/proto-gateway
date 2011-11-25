#include "bacnetcommon.h"

using namespace Bacnet;

Bacnet::ObjIdNum Bacnet::objIdToNum(const Bacnet::ObjectIdStruct &objId) {
    return ( (objId.objectType << 22) | (0x3fffff & objId.instanceNum) );
}

Bacnet::ObjectIdStruct Bacnet::numToObjId(Bacnet::ObjIdNum numObjId) {
    Bacnet::ObjectIdStruct objIdStr = {
        (BacnetObjectTypeNS::ObjectType)(numObjId >> 22),
        numObjId & 0x3fffff
    };

    return objIdStr;
}

const Bacnet::ObjectIdStruct &Bacnet::invalidObjectId() {
    static ObjectIdStruct invlaidStruct = {BacnetObjectTypeNS::Undefined, InvalidInstanceNumber};
    return invlaidStruct;
}

Bacnet::ObjIdNum Bacnet::invalidObjIdNum() {
    return objIdToNum(invalidObjectId());
}

uint Bacnet::qHash(ObjectIdStruct objId)
{
    return objIdToNum(objId);
}

bool Bacnet::operator==(ObjectIdStruct one, ObjectIdStruct other)
{
    return objIdToNum(one) == objIdToNum(other);
}
