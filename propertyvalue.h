#ifndef PROPERTYVALUE_H
#define PROPERTYVALUE_H

#include "bacnetdata.h"
#include <QSharedPointer>

namespace Bacnet {

class PropertyValue;
typedef QSharedPointer<PropertyValue> PropertyValueShared;

    class PropertyValue
    {
    public:
        PropertyValue();
        PropertyValue(BacnetProperty::Identifier propertyId, BacnetDataInterface *value,
                      quint32 arrayIndex = ArrayIndexNotPresent, quint8 priority = PriorityValueNotPresent);
        PropertyValue(BacnetProperty::Identifier propertyId, BacnetDataInterfaceShared value,
                      quint32 arrayIndex = ArrayIndexNotPresent, quint8 priority = PriorityValueNotPresent);

        virtual ~PropertyValue();

        qint32 toRaw(quint8 *ptrStart, quint16 buffLength, int sequenceShift = 0);

    public:
        qint32 fromRawSpecific(BacnetTagParser &parser, BacnetObjectType::ObjectType objType, int sequenceShift = 0);
        qint32 fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectType::ObjectType objType);

    public:


    public:
//        Bacnet::PropertyValueStruct _value;
        BacnetProperty::Identifier _propertyId;
        quint32 _arrayIndex;
        BacnetDataInterfaceShared _value;
        quint8 _priority;
    };
}
#endif // PROPERTYVALUE_H
