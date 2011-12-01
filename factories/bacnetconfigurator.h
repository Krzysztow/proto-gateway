#ifndef BACNET_OBJECTSSTRUCTUREFACTORY_H
#define BACNET_OBJECTSSTRUCTUREFACTORY_H

#include <QDomDocument>
#include <QString>
#include <QMap>
#include "bacnetcommon.h"
#include "bacnetpci.h"

class DataModel;

namespace Bacnet {

class BacnetDeviceObject;
class BacnetObject;
class BacnetProperty;
class InternalPropertyContainerSupport;
class InternalObjectsHandler;
class ExternalObjectsHandler;
class ObjectIdentifier;
class ExternalObjectReadStrategy;
class ExternalObjectWriteStrategy;
class BacnetApplicationLayerHandler;

class BacnetConfigurator
{
public:
    void configureInternalHandler(QDomElement &devicesConfig, DataModel *dataModel, InternalObjectsHandler *intHandler);
    void configureExternalHandler(QDomElement extPropsConfig, DataModel *dataModel, BacnetApplicationLayerHandler *appLayer);
    void configureDeviceMappings(QDomElement &mappings, BacnetApplicationLayerHandler *appLayer);

    static BacnetConfigurator *instance();
    static void releaseInstance();

private:
    BacnetDeviceObject *createDevice(QDomElement &deviceElement);
    void populateWithProperties(BacnetObject *object, QDomElement &propertiesRootElement);
    void createObject(BacnetDeviceObject *toBeParentDevice, QDomElement &objectElement);
    BacnetProperty *createAbstractProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport);
    BacnetProperty *createSimpleProperty(QDomElement &propElem);
    ExternalObjectReadStrategy *createReadStrategy(QDomElement &pElem);
    ExternalObjectWriteStrategy *createWriteStrategy(QDomElement &pElem);

    BacnetProperty *createInternalProxyProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport);

    void populatePropertyMappings(ObjectIdentifier &deviceId, ObjectIdentifier &objectId, QDomElement &objElement, BacnetApplicationLayerHandler *appLayer);

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
    QMap<QString, Bacnet::BacnetSegmentation> segmentDict();
    QVariant qvariantFromValue(QString value, TagConversion types, bool *doneOk);


private:
    BacnetConfigurator();
    static BacnetConfigurator *_instance;
    DataModel *_dataModel;

    QMap<QString, TagConversion> _typesMap;
    QMap<QString, BacnetObjectTypeNS::ObjectType> _supportedObjectsTypes;
    QMap<QString, Bacnet::BacnetSegmentation> _segmentationDictionary;
};

} // namespace Bacnet

#endif // BACNET_OBJECTSSTRUCTUREFACTORY_H
