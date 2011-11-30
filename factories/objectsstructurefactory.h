#ifndef BACNET_OBJECTSSTRUCTUREFACTORY_H
#define BACNET_OBJECTSSTRUCTUREFACTORY_H

#include <QDomDocument>
#include <QString>
#include <QMap>
#include "bacnetcommon.h"

class DataModel;

namespace Bacnet {

class BacnetDeviceObject;
class BacnetObject;
class BacnetProperty;
class InternalPropertyContainerSupport;

class ObjectsStructureFactory
{
public:
    QList<BacnetDeviceObject*> createDevicesFromConfig(QDomElement &devicesConfig, DataModel *dataModel);
    static ObjectsStructureFactory *instance();
    static void releaseInstance();

private:
    BacnetDeviceObject *createDevice(QDomElement &deviceElement);
    void populateWithProperties(BacnetObject *object, QDomElement &propertiesRootElement);
    void createObject(BacnetDeviceObject *toBeParentDevice, QDomElement &objectElement);
    BacnetProperty *createAbstractProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport);
    BacnetProperty *createSimpleProperty(QDomElement &propElem);
    BacnetProperty *createInternalProxyProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport);

    class TagConversion
    {
    public:
        TagConversion(AppTags::BacnetTags bacType, uint varType):
            bacnetType(bacType),
            variantType(varType) {}

    public:
        AppTags::BacnetTags bacnetType;
        uint variantType;
    };

    QMap<QString, TagConversion> typesMap();
    QMap<QString, BacnetObjectTypeNS::ObjectType> supportedObjectsTypes();
    QVariant qvariantFromValue(QString value, TagConversion types, bool *doneOk);


private:
    ObjectsStructureFactory();
    static ObjectsStructureFactory *_instance;
    DataModel *_dataModel;

    QMap<QString, TagConversion> _typesMap;
    QMap<QString, BacnetObjectTypeNS::ObjectType> _supportedObjectsTypes;
};

} // namespace Bacnet

#endif // BACNET_OBJECTSSTRUCTUREFACTORY_H
