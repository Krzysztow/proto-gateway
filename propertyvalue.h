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
        PropertyValue(BacnetPropertyNS::Identifier propertyId, BacnetDataInterface *value,
                      quint32 arrayIndex = ArrayIndexNotPresent, quint8 priority = PriorityValueNotPresent);
        PropertyValue(BacnetPropertyNS::Identifier propertyId, BacnetDataInterfaceShared value,
                      quint32 arrayIndex = ArrayIndexNotPresent, quint8 priority = PriorityValueNotPresent);

        virtual ~PropertyValue();

        qint32 toRaw(quint8 *ptrStart, quint16 buffLength, int sequenceShift = 0);

    public:
        qint32 fromRawSpecific(BacnetTagParser &parser, BacnetObjectTypeNS::ObjectType objType, int sequenceShift = 0);
        qint32 fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectTypeNS::ObjectType objType);

    public:


    public:
//        Bacnet::PropertyValueStruct _value;
        BacnetPropertyNS::Identifier _propertyId;
        quint32 _arrayIndex;
        BacnetDataInterfaceShared _value;
        quint8 _priority;
    };
}
#endif // PROPERTYVALUE_H
