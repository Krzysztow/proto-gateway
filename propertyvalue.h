#ifndef PROPERTYVALUE_H
#define PROPERTYVALUE_H

#include "bacnetdata.h"

namespace Bacnet {
    class PropertyValue
    {
    public:
        PropertyValue();
        virtual ~PropertyValue();

        qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

    public:
        qint32 fromRawSpecific(BacnetTagParser &parser, BacnetObjectType::ObjectType objType);
        qint32 fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectType::ObjectType objType);


    private:
        Bacnet::PropertyValueStruct _value;
    };
}
#endif // PROPERTYVALUE_H
