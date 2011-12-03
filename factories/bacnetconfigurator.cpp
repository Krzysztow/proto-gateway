#include "bacnetconfigurator.h"

#include "bacnetcommon.h"
#include "bacnetdeviceobject.h"
#include "bacnetinternaladdresshelper.h"
#include "bacnetproperty.h"
#include "bacnetdefaultobject.h"
#include "internalobjectshandler.h"
#include "bacnetapplicationlayer.h"
#include "externalobjectreadstrategy.h"
#include "externalobjectwritestrategy.h"
#include "cdm.h"
#include "configuratorhelper.h"

using namespace Bacnet;

static const char *DevicesListTagName               = "devices";
static const char *BacnetDeviceTagName              = "device";
static const char *DeviceInternalAddressAttribute   = "int-address";
static const char *DeviceInstanceNumberAttribute    = "dev-instance-number";
static const char *DeviceObjectsListTagName         = "childObjects";
static const char *BacnetObjectTagName              = "object";

static const char *ObjectPropertiesTagName          = "properties";
static const char *ObjectPropertyTagName            = "property";
static const char *ObjectNameAttribute              = "name";
static const char *ObjectInstanceNumberAttribute    = "instance-number";
static const char *ObjectTypeAttribute              = "obj-type";

static const char *BacnetPropertySimpleType         = "simple";
static const char *BacnetPropertyArray              = "array";
static const char *BacnetPropertyIntProxy           = "internal";

static const char *PropertyArrayIdxAttribute        = "array-idx";
static const char *PropertyValueAttribute           = "value";
static const char *PropertyPropertyTypeAttribute    = "prop-type";
static const char *PropertyBacnetTypeAttribute      = "bac-type";
static const char *PropertyIdentifierAttribute      = "id";

static const char *BacnetDeviceAddressAttribute     = "bac-address";
static const char *BacnetDeviceNetworkAttribute     = "bac-network";
static const char *BacnetDeviceSegmentationAttribute= "bac-segmentation";
static const char *BacnetDeviceMaxApduAttribute     = "bac-max-apdu";

void BacnetConfigurator::configureInternalHandler(QDomElement &devicesConfig, DataModel *dataModel, InternalObjectsHandler *intHandler)
{
    _dataModel = dataModel;
    Q_CHECK_PTR(_dataModel);
    
    QDomNodeList devicesList = devicesConfig.elementsByTagName(DevicesListTagName).at(0).toElement().elementsByTagName(BacnetDeviceTagName);
    int devicesNumber = devicesList.count();
    QDomElement deviceElement;
    BacnetDeviceObject *device;
    
    QList<BacnetAddress> devicesAddresses;
    QList<ObjIdNum> instances;
    QList<BacnetDeviceObject*> devices;
    
    for (int i = 0; i < devicesNumber; ++i) {
        deviceElement = devicesList.at(i).toElement();
        device = createDevice(deviceElement);
        if ( (0 != device) &&
             (!devicesAddresses.contains(device->address())) &&     //no address duplication
             (!instances.contains(device->objectIdNum())) ){        //no instance number duplication
            devicesAddresses.append(device->address());
            instances.append(device->objectIdNum());
            devices.append(device);
        } else
            delete device;
    }
    
    foreach (BacnetDeviceObject *device, devices) {
        intHandler->addDevice(device->address(), device);
    }
}

void BacnetConfigurator::configureExternalHandler(QDomElement extPropsConfig, DataModel *dataModel, BacnetApplicationLayerHandler *appLayer)
{
    _dataModel = dataModel;
    Q_CHECK_PTR(_dataModel);

    bool ok;
    //first set internal address - if the address is wrong, we are not going to work!
    ExternalObjectsHandler *extHandler = appLayer->externalHandler();
    Q_CHECK_PTR(extHandler);
    InternalAddress extAddress = extPropsConfig.attribute(DeviceInternalAddressAttribute).toUInt(&ok);
    if (!ok) {
#warning "External address not could be not set!";
        for (int i = 0; i < 10; ++i) {
            extAddress = qrand();
            if (!appLayer->internalHandler()->virtualDevices().contains(extAddress)) {
                QString info = QString("Did not got valid address, will use generated one %1").arg(extAddress);
                elementError(extPropsConfig, DeviceInternalAddressAttribute, qPrintable(info));
            }
        }
    }
    extHandler->addRegisteredAddress(extAddress);

    QDomNodeList devicesList = extPropsConfig.elementsByTagName(DevicesListTagName).at(0).toElement().elementsByTagName(BacnetDeviceTagName);
    int devicesNumber = devicesList.count();
    QDomElement deviceElement;

    ObjectIdentifier deviceId;
    ObjectIdentifier objectId;
    QString str;
    for (int i = 0; i < devicesNumber; ++i) {
        deviceElement = devicesList.at(i).toElement();
        deviceId.setObjectIdNum(deviceElement.attribute(DeviceInstanceNumberAttribute).toUInt(&ok, 0));
        if (!ok)
            continue;
        if (deviceId.type() != BacnetObjectTypeNS::Device)
            deviceId.setObjectId(BacnetObjectTypeNS::Device, deviceId.instanceNumber());
        QDomElement childsElement = deviceElement.firstChildElement(DeviceObjectsListTagName);
        for (QDomElement objElement = childsElement.firstChildElement(BacnetObjectTagName); !objElement.isNull(); objElement = objElement.nextSiblingElement(BacnetObjectTagName)) {
            str = objElement.attribute(ObjectTypeAttribute);
            if (!_supportedObjectsTypes.contains(str)) {
                elementError(objElement, ObjectTypeAttribute, "Unsupported type.");
                continue;
            }
            objectId.setObjectId(_supportedObjectsTypes[str],
                                 objElement.attribute(ObjectInstanceNumberAttribute).toUInt(&ok, 0));
            populatePropertyMappings(deviceId, objectId, objElement, appLayer);
        }
    }
}

void BacnetConfigurator::populatePropertyMappings(ObjectIdentifier &deviceId, ObjectIdentifier &objectId, QDomElement &objElement, BacnetApplicationLayerHandler *appLayer)
{
    BacnetPropertyNS::Identifier propertyId;
    quint32 propArrayIdx(ArrayIndexNotPresent);
    bool ok;
    for (QDomElement propsElem = objElement.firstChildElement(ObjectPropertiesTagName); !propsElem.isNull(); propsElem = propsElem.nextSiblingElement(ObjectPropertiesTagName)) {
        for (QDomElement pElem = propsElem.firstChildElement(ObjectPropertyTagName); !pElem.isNull(); pElem = pElem.nextSiblingElement(ObjectPropertyTagName)) {
            //get property ID - necessary
            propertyId = (BacnetPropertyNS::Identifier)pElem.attribute(PropertyIdentifierAttribute).toUInt(&ok);
            if (!ok) {
                elementError(pElem, PropertyIdentifierAttribute);
                continue;
            }

            //get array idx - not necessary
            if (pElem.hasAttribute(PropertyArrayIdxAttribute)) {
                propArrayIdx = pElem.attribute(PropertyArrayIdxAttribute).toUInt(&ok);
                if (!ok) {
                    elementError(pElem, PropertyArrayIdxAttribute);
                    continue;
                }
            } else
                propArrayIdx = ArrayIndexNotPresent;

            //create property subject
            PropertySubject *property = _dataModel->createPropertySubject(pElem);
            if (0 == property) {
                qDebug("%s : Cannot create property subject.", __PRETTY_FUNCTION__);
                continue;
            }

            //create read and write strategies
            ExternalObjectReadStrategy *readStrategy = createReadStrategy(pElem);
            ExternalObjectWriteStrategy *writeStrategy = createWriteStrategy(pElem);
            if (0 == readStrategy || 0 == writeStrategy) {
                delete property;
                continue;
            }

            ExternalObjectsHandler *extHandler = appLayer->externalHandler();
            Q_CHECK_PTR(extHandler);
            extHandler->addMappedProperty(property, objectId.objectIdNum(), propertyId, propArrayIdx, readStrategy, writeStrategy);
            //if we are here, add object - device mapping
            appLayer->registerObject(deviceId, objectId);
        }
    }
}

const char *ReadStrategyAttribute       = "read-strategy";
const char *SimpleReadStrategyName      = "simple";
const char *SimpleTimeReadStrategyName  = "simple-time";
const char *CovReadStrategyName         = "cov";
const char *CovPollReadStrategyName     = "cov-poll";
const char *ReadStrategyIntervalAttribute   = "read-interval";
const char *CovConfirmedAttribute       = "cov-confirmed";
const char *SimpleWriteStrategyName      = "simple";
const char *WriteStrategyAttribute      = "write-strategy";

ExternalObjectWriteStrategy *BacnetConfigurator::createWriteStrategy(QDomElement &pElem)
{
    QString str = pElem.attribute(WriteStrategyAttribute);
    if (SimpleWriteStrategyName == str)
        return new ExternalObjectWriteStrategy();
    else
        return 0;
}

ExternalObjectReadStrategy *BacnetConfigurator::createReadStrategy(QDomElement &pElem)
{
    QString str = pElem.attribute(ReadStrategyAttribute);
    bool ok;
    int interval;
    if (SimpleReadStrategyName == str) {
        //no additional parameters are required
        return new SimpleReadStrategy();
    } else {
        //! interval is optional - if not provided, we create simple read strategy.
        if (pElem.hasAttribute(ReadStrategyIntervalAttribute)) {
            interval = pElem.attribute(ReadStrategyIntervalAttribute).toUInt(&ok);
            if (!ok) {
                elementError(pElem, ReadStrategyIntervalAttribute,"Will create simple strategy.");
                return new SimpleReadStrategy();
            }
        }

        if (interval < 1000)
            qDebug("%s : WARNING: Read interval is too small (%d ms).", __PRETTY_FUNCTION__, interval);

        if (SimpleTimeReadStrategyName == str) {
            return new SimpleWithTimeReadStrategy(interval);
        } else if (CovReadStrategyName == str || CovPollReadStrategyName == str) {
            //optional - confirmed, interval (resubscription or reading on error), reading on error. If not provided, by default is false, 60000ms, false.
            bool confirmed(false);
            bool pollOnError((CovPollReadStrategyName == str) ? true : false);
            if (pElem.hasAttribute(CovConfirmedAttribute)) {
                if ("true" == pElem.attribute(CovConfirmedAttribute).toLower())
                    confirmed = false;
            }

            return new CovReadStrategy(interval, confirmed, pollOnError);
        }
    }
    return 0;
}

//QDomNode getFirstDirecChildElementByName(QDomElement &node, char *tagName)
//{
//    QDomNodeList nodes = node.elementsByTagName(tagName);
//    QDomElement foundNode;
//    for (int i = 0; i < nodes.count(); ++i) {
//        foundNode = nodes.at(i).toElement();
//        if (foundNode.parentNode() == node && foundNode.tagName() == tagName)
//            return foundNode;
//    }
    
//    return QDomNode();
//}

BacnetDeviceObject *BacnetConfigurator::createDevice(QDomElement &deviceElement)
{
    bool ok(true);
    //get address
    BacnetAddress address = BacnetInternalAddressHelper::toBacnetAddress(deviceElement.attribute(DeviceInternalAddressAttribute).toUInt(&ok));
    if (!ok || !address.isAddrInitialized() ) {
        elementError(deviceElement, DeviceInternalAddressAttribute, "Address can't be parsed.");
        return 0;
    }
    //get device instance number
    quint32 instanceNum = deviceElement.attribute(DeviceInstanceNumberAttribute).toUInt(&ok);
    if (!ok) {
        elementError(deviceElement, DeviceInstanceNumberAttribute, "Can't create or reserved.");
        return 0;
    }
    
    //create device
    BacnetDeviceObject *device = new BacnetDeviceObject(instanceNum, address);
    
    //set name OPTIONAL
    if (deviceElement.hasAttribute(ObjectNameAttribute))
        device->setObjectName(deviceElement.attribute(ObjectNameAttribute));
    
    BacnetObject *childObject(0);
    for (QDomElement el = deviceElement.firstChildElement(); !el.isNull(); el = el.nextSiblingElement()) {
        if (el.tagName() == ObjectPropertiesTagName) {          //add properties
            populateWithProperties(device, el);
        } else if (el.tagName() == DeviceObjectsListTagName) {  //add objects
            QDomNodeList childObjectsList = el.elementsByTagName(BacnetObjectTagName);
            for (int i = 0; i < childObjectsList.count(); ++i) {
                QDomElement objectElement = childObjectsList.at(i).toElement();
                createObject(device, objectElement);
                if (0 != childObject) {
#warning "Do I have to make some checking against object instance number duplication?"
                    device->addBacnetObject(childObject);
                }
            }
        }
    }
    
    return device;
}

void BacnetConfigurator::createObject(BacnetDeviceObject *toBeParentDevice, QDomElement &objectElement)
{
    //get device instance number
    bool ok;
    quint32 instanceNum = objectElement.attribute(ObjectInstanceNumberAttribute).toUInt(&ok);
    if (!ok ) {
        elementError(objectElement, ObjectInstanceNumberAttribute);
        return;
    }
    
    QString objectTypeStr = objectElement.attribute(ObjectTypeAttribute).toLower();
    
    if (!_supportedObjectsTypes.contains(objectTypeStr)) {
        qDebug("%s : Object type %s not supported.", __PRETTY_FUNCTION__, qPrintable(objectTypeStr));
        return;
    }
    
    BacnetObjectTypeNS::ObjectType objectType = _supportedObjectsTypes[objectTypeStr];
    BacnetObject *object = new BacnetObject(objectType, instanceNum, toBeParentDevice);
    //! \todo \warning If by mistake there was another object with same id number (type + instance), we would have a memory leak, since will not be added to object.
    
    if (objectElement.hasAttribute(ObjectNameAttribute))
        object->setObjectName(objectElement.attribute(ObjectNameAttribute));
    
    for (QDomElement el = objectElement.firstChildElement(ObjectPropertiesTagName); !el.isNull(); el = el.nextSiblingElement(ObjectPropertiesTagName)) { //note we allow only for properties tag
        populateWithProperties(object, el);
    }
}



void BacnetConfigurator::populateWithProperties(BacnetObject *object, QDomElement &propertiesRootElement)
{
    bool ok;
    for (QDomElement pElem = propertiesRootElement.firstChildElement(ObjectPropertyTagName); !pElem.isNull(); pElem = pElem.nextSiblingElement(ObjectPropertyTagName)) {
        //required attributes - property type and property identifier
        if ( !pElem.hasAttribute(PropertyPropertyTypeAttribute) ||
             !pElem.hasAttribute(PropertyIdentifierAttribute) ) {
            elementError(pElem, PropertyPropertyTypeAttribute);
            continue;
        }
        
        //get property identifier
        BacnetPropertyNS::Identifier propId = (BacnetPropertyNS::Identifier)(pElem.attribute(PropertyIdentifierAttribute).toUInt(&ok));
        if (!ok) {
            elementError(pElem, PropertyIdentifierAttribute);
            continue;
        }
        
        BacnetProperty *property = createAbstractProperty(pElem, object);
        if ((0 == property) || !object->addProperty(propId, property))
            delete property;
    }
}

BacnetProperty *BacnetConfigurator::createAbstractProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport)
{
    //get property type
    QString propType = propElem.attribute(PropertyPropertyTypeAttribute);
    if (BacnetPropertySimpleType == propType) {
        
        //this property type requires bacnet-type and its value
        if (!propElem.hasAttribute(PropertyBacnetTypeAttribute) || !propElem.hasAttribute(PropertyValueAttribute)) {
            qDebug("%s : simple property has no required parameters!", __PRETTY_FUNCTION__);
            return 0;
        }
        return createSimpleProperty(propElem);
        
    } else if (BacnetPropertyArray == propType) {
        
        QList<BacnetProperty*> _propertiesList;
        ArrayProperty *property = new ArrayProperty(_propertiesList, containerSupport);
        for (QDomElement childPropElem = propElem.firstChildElement(ObjectPropertyTagName); !childPropElem.isNull(); childPropElem = childPropElem.nextSiblingElement(ObjectPropertyTagName)) {
            BacnetProperty *childProperty = createAbstractProperty(childPropElem, property);
            if (0 != childProperty)
                _propertiesList.append(childProperty);
        }
        return property;
        
    } else if (BacnetPropertyIntProxy == propType) {
        
        if (!propElem.hasAttribute(PropertyBacnetTypeAttribute)) {
            elementError(propElem, PropertyBacnetTypeAttribute);
            return 0;
        }
        return createInternalProxyProperty(propElem, containerSupport);
        
    }
    
    return 0;
}

BacnetProperty *BacnetConfigurator::createInternalProxyProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport)
{
    //get BacnetApp tag
    QMap<QString, BacnetConfigurator::TagConversion>::Iterator typeIt = _typesMap.find(propElem.attribute(PropertyBacnetTypeAttribute));
    if (typeIt == _typesMap.end()) {
        elementError(propElem, PropertyBacnetTypeAttribute);
        return 0;
    }
    ::Property *intenralProperty = _dataModel->createProperty(propElem);
    if (0 == intenralProperty)
        return 0;
    
    return new ProxyInternalProperty(intenralProperty, typeIt->bacnetType, intenralProperty->type(), containerSupport);
}

BacnetProperty *BacnetConfigurator::createSimpleProperty(QDomElement &propElem)
{
    bool ok;
    
    //get BacnetApp tag
    QMap<QString, BacnetConfigurator::TagConversion>::Iterator typeIt = _typesMap.find(propElem.attribute(PropertyBacnetTypeAttribute));
    if (typeIt == _typesMap.end()) {
        elementError(propElem, PropertyBacnetTypeAttribute);
        return 0;
    }
    
    //convert string value to bacnet app tag acceptable
    QVariant value = qvariantFromValue(propElem.attribute(PropertyValueAttribute), typeIt.value(), &ok);
    if (!ok) {
        qDebug("%s : can't convert value %s.", __PRETTY_FUNCTION__, qPrintable(propElem.attribute(PropertyValueAttribute)));
        return 0;
    }
    
    //create data
    BacnetDataInterface *data = BacnetDefaultObject::createDataProperty(typeIt->bacnetType, &value, &ok);
    if (!ok || (0 == data)) {
        qDebug("%s : can't create property data", __PRETTY_FUNCTION__);
        delete data;
        return 0;
    }
    //wrapp into bacnetproperty
    return new SimpleProperty(data);
}

QMap<QString, BacnetObjectTypeNS::ObjectType> BacnetConfigurator::supportedObjectsTypes()
{
    QMap<QString, BacnetObjectTypeNS::ObjectType> supportedObjects;
    
    supportedObjects.insert("analog-input", BacnetObjectTypeNS::AnalogInput);
    supportedObjects.insert("analog-output", BacnetObjectTypeNS::AnalogOutput);
    supportedObjects.insert("analog-value", BacnetObjectTypeNS::AnalogValue);
    supportedObjects.insert("binary-input", BacnetObjectTypeNS::BinaryInput);
    supportedObjects.insert("binary-output", BacnetObjectTypeNS::BinaryOutput);
    supportedObjects.insert("binary-value", BacnetObjectTypeNS::BinaryValue);
    supportedObjects.insert("multistate-input", BacnetObjectTypeNS::MultiStateInput);
    supportedObjects.insert("multistate-output",BacnetObjectTypeNS::MultiStateOutput);
    supportedObjects.insert("multistate-value", BacnetObjectTypeNS::MultiStateValue);
    
    return supportedObjects;
}

QMap<QString, BacnetConfigurator::TagConversion> BacnetConfigurator::typesMap()
{
    QMap<QString, TagConversion> tm;
    
    tm.insert("null", TagConversion(AppTags::Null, 0));
    tm.insert("boolean", TagConversion(AppTags::Boolean, QVariant::Bool));
    tm.insert("unsignedinteger", TagConversion(AppTags::UnsignedInteger, QVariant::UInt));
    tm.insert("signedinteger", TagConversion(AppTags::SignedInteger, QVariant::Int));
    tm.insert("real", TagConversion(AppTags::Real, QMetaType::Float));
    tm.insert("double", TagConversion(AppTags::Double, QVariant::Double));
    tm.insert("octetstring", TagConversion(AppTags::OctetString, QVariant::ByteArray));
    tm.insert("characterstring", TagConversion(AppTags::CharacterString, QVariant::String));
    tm.insert("bitstring", TagConversion(AppTags::BitString, QVariant::BitArray));
    tm.insert("enumerated", TagConversion(AppTags::Enumerated, QVariant::UInt));
    tm.insert("date", TagConversion(AppTags::Date, QVariant::Date));
    tm.insert("time", TagConversion(AppTags::Time, QVariant::Time));
    tm.insert("bacnetobjectidentifier", TagConversion(AppTags::BacnetObjectIdentifier, QVariant::UInt));
    
    return tm;
}

BacnetConfigurator *BacnetConfigurator::_instance = 0;
BacnetConfigurator *BacnetConfigurator::instance()
{
    if (0 == _instance)
        _instance = new BacnetConfigurator();
    return _instance;
}

BacnetConfigurator::BacnetConfigurator():
    _typesMap(typesMap()),
    _supportedObjectsTypes(supportedObjectsTypes()),
    _segmentationDictionary(segmentDict())
{
    
}

void BacnetConfigurator::releaseInstance()
{
    delete _instance;
    _instance = 0;
}

QVariant BacnetConfigurator::qvariantFromValue(QString value, TagConversion types, bool *doneOk)
{
    bool ok;
    QVariant var;
    switch (types.bacnetType) {
    case (AppTags::Null): {
        //*/return new Bacnet::Null(var);
    }
        break;
    case (AppTags::Boolean):
    case (AppTags::UnsignedInteger):
    case (AppTags::SignedInteger):
    case (AppTags::Real):
    case (AppTags::Double):
    case (AppTags::CharacterString):
    case (AppTags::Enumerated):
    case (AppTags::Date):
    case (AppTags::Time):
    {
        var = QVariant::fromValue(value);
        ok = var.convert((QVariant::Type)types.variantType);
    }
        break;
    case (AppTags::BacnetObjectIdentifier): {
        uint id = value.toUInt(&ok, 0);
        var = id;
    }
        break;
    case (AppTags::OctetString): {
        QStringList valuesList = value.split(",");
        QByteArray vArray;
        vArray.reserve(valuesList.count());
        int i = 0;
        foreach (QString v, valuesList) {
            vArray[i] = v.toUInt(&ok, 0);
            if (!ok)
                break;
            i++;
        }
        var = vArray;
    }
        break;
    case (AppTags::BitString): {
        QString::Iterator it = value.begin();
        int bits = value.size();
        if ('b' == *it || 'B' == *it) {
            it++;
            --bits;
        }
        QBitArray ba(bits);
        --bits;
        for (; it!=value.end(); ++it) {
            if ('1' == *it)
                ba.setBit(bits, true);
            --bits;
        }
        var = ba;
    }
        break;
    default:
        ok = false;
        Q_ASSERT(false);
    }
    
    if (0 != doneOk)
        *doneOk = ok;
    return var;
}

void BacnetConfigurator::configureDeviceMappings(QDomElement &mappings, Bacnet::BacnetApplicationLayerHandler *appLayer)
{
    Q_CHECK_PTR(appLayer);

    QDomNodeList mappedDevices = mappings.elementsByTagName(BacnetDeviceTagName);
    QDomElement devElem;

    ObjectIdentifier devId;
    BacnetAddress devAddress;
    int maxApduLength;
    Bacnet::BacnetSegmentation devSegmentation;

    bool ok;
    int devicesCount = mappedDevices.count();
    for (int i = 0; i < devicesCount; ++i) {
        devElem = mappedDevices.at(i).toElement();

        //get device identifier
        devId = devElem.attribute(DeviceInstanceNumberAttribute).toUInt(&ok, 0);
        if (!ok) {
            elementError(devElem, DeviceInstanceNumberAttribute);
            continue;
        }
        if (devId.type() != BacnetObjectTypeNS::Device)
            devId.setObjectId(BacnetObjectTypeNS::Device, devId.instanceNumber());

        //get device bacnet mac address
        QString str = devElem.attribute(BacnetDeviceAddressAttribute);
        if (str.isEmpty() || !devAddress.macAddressFromString(str)) {
            elementError(devElem, BacnetDeviceAddressAttribute);
            continue;
        }

        //get device network
        str = devElem.attribute(BacnetDeviceNetworkAttribute);
        if (!str.isEmpty())
                if (!devAddress.networkNumFromString(str)) {
                    elementError(devElem, BacnetDeviceNetworkAttribute);
                    continue;
                }

        //get device max length accepted
        maxApduLength = devElem.attribute(BacnetDeviceMaxApduAttribute).toUInt(&ok);
        if (!ok) {
            elementError(devElem, BacnetDeviceMaxApduAttribute);
            continue;
        }

        //get segmentation type
        str = devElem.attribute(BacnetDeviceSegmentationAttribute);
        if (_segmentationDictionary.contains(str))
            devSegmentation = _segmentationDictionary[str];
        else {
            elementError(devElem, BacnetDeviceSegmentationAttribute, "Using segmented-not!");
            devSegmentation = Bacnet::SegmentedNOT;
        }

        appLayer->registerDevice(devAddress, devId, maxApduLength, devSegmentation);
    }
}


QMap<QString, Bacnet::BacnetSegmentation> BacnetConfigurator::segmentDict()
{
    QMap<QString, Bacnet::BacnetSegmentation> mSegsDictionary;
    mSegsDictionary.insert("segmented-not",     Bacnet::SegmentedNOT);
    mSegsDictionary.insert("segmented-both",    Bacnet::SegmentedBoth);
    mSegsDictionary.insert("segmented-tx",      Bacnet::SegmentedTransmit);
    mSegsDictionary.insert("segmented-rx",      Bacnet::SegmentedReceive);

    return mSegsDictionary;
}

static const char *InternalHandlerTagName   = "internalHandler";
static const char *ExternalHandlerTagName   = "externalHandler";
static const char *DeviceMappingsTagName    = "deviceMappings";

static const char *AppLayerNetNumber        = "net-num";

#include "bacnetnetworklayer.h"

BacnetApplicationLayerHandler *BacnetConfigurator::createApplicationLayer(BacnetNetworkLayerHandler *netHandler, QDomElement &appCfg)
{
    Q_CHECK_PTR(netHandler);

    bool ok;
    quint32 netNumber = appCfg.attribute(AppLayerNetNumber).toUInt(&ok);
    if (!ok) {
        elementError(appCfg, AppLayerNetNumber, "No network number set, exit.");
        return 0;
    }


    BacnetApplicationLayerHandler *appHandler = new BacnetApplicationLayerHandler(netHandler);
    netHandler->setVirtualApplicationLayer(netNumber, appHandler);

    InternalObjectsHandler *intHandler = appHandler->internalHandler();
    Q_CHECK_PTR(intHandler);

    QDomElement el = appCfg.firstChildElement(InternalHandlerTagName);
    BacnetConfigurator::instance()->configureInternalHandler(el, DataModel::instance(), intHandler);

    el = appCfg.firstChildElement(ExternalHandlerTagName);
    BacnetConfigurator::instance()->configureExternalHandler(el, DataModel::instance(), appHandler);

    el = appCfg.firstChildElement(DeviceMappingsTagName);
    BacnetConfigurator::instance()->configureDeviceMappings(el, appHandler);

    BacnetConfigurator::releaseInstance();



    return appHandler;
}


