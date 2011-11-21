#ifndef BACNETDEFAULTOBJECT_H
#define BACNETDEFAULTOBJECT_H

#include "bacnetdata.h"
#include "bacnetcommon.h"

namespace Bacnet {

class BacnetProperty;

class BacnetDefaultObject
{
public:
    static BacnetDefaultObject *instance();

    static quint32 proeprtyType(BacnetObjectTypeNS::ObjectType type, BacnetPropertyNS::Identifier propertyId, quint32 arrayIdx = ArrayIndexNotPresent);

    static Bacnet::BacnetDataInterface *createDataProperty(AppTags::BacnetTags propertyType, QVariant *value = 0, bool *ok = 0);


    static Bacnet::BacnetDataInterface *createDataForObjectProperty(BacnetObjectTypeNS::ObjectType type,
                                                                    BacnetPropertyNS::Identifier propertyId, quint32 arrayIndex);

    static Bacnet::BacnetDataInterface *createDataType(DataType::DataType type);

    static QList<BacnetPropertyNS::Identifier> covProperties(BacnetObjectTypeNS::ObjectType type);

public:
    BacnetProperty *defaultProperty(BacnetObjectTypeNS::ObjectType objType, BacnetPropertyNS::Identifier propId);
    QMap<BacnetPropertyNS::Identifier, BacnetProperty*> &defaultProperties(BacnetObjectTypeNS::ObjectType objType);

private:
    BacnetDefaultObject();
    static BacnetDefaultObject *_instance;

    QMap<BacnetObjectTypeNS::ObjectType, QMap<BacnetPropertyNS::Identifier, BacnetProperty*> > _properties;
};
}
#endif // BACNETDEFAULTOBJECT_H
