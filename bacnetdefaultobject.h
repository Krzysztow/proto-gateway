#ifndef BACNETDEFAULTOBJECT_H
#define BACNETDEFAULTOBJECT_H

#include "bacnetdata.h"
#include "bacnetcommon.h"

namespace Bacnet {
    class BacnetDefaultObject
    {
    public:
        static BacnetDefaultObject *instance();

        static quint32 proeprtyType(BacnetObjectType::ObjectType type, BacnetProperty::Identifier propertyId);

        static Bacnet::BacnetDataInterface *createDataForObjectProperty(BacnetObjectType::ObjectType type,
                                                                                   BacnetProperty::Identifier propertyId, quint32 arrayIndex);

        static Bacnet::BacnetDataInterface *createDataType(DataType::DataType type);

    public:
        Bacnet::BacnetDataInterface *getProperty(BacnetObjectType::ObjectType objType, BacnetProperty::Identifier propId);
        const QMap<BacnetProperty::Identifier, Bacnet::BacnetDataInterface*> &defaultProperties(BacnetObjectType::ObjectType objType);

    private:
        BacnetDefaultObject();
        static BacnetDefaultObject *_instance;

        QMap<BacnetObjectType::ObjectType, QMap<BacnetProperty::Identifier, Bacnet::BacnetDataInterface*> > _properties;
    };
}
#endif // BACNETDEFAULTOBJECT_H
