#include "objectsstructurefactory.h"

#include "bacnetcommon.h"
#include "bacnetdeviceobject.h"
#include "bacnetinternaladdresshelper.h"
#include "bacnetproperty.h"
#include "bacnetdefaultobject.h"
#include "cdm.h"

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

QList<BacnetDeviceObject *> ObjectsStructureFactory::createDevicesFromConfig(QDomElement &devicesConfig, DataModel *dataModel)
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

    //it's fine to return non-reference, since QList is implicitly shared.
    return devices;
}

QDomNode getFirstDirecChildElementByName(QDomElement &node, char *tagName)
{
    QDomNodeList nodes = node.elementsByTagName(tagName);
    QDomElement foundNode;
    for (int i = 0; i < nodes.count(); ++i) {
        foundNode = nodes.at(i).toElement();
        if (foundNode.parentNode() == node && foundNode.tagName() == tagName)
            return foundNode;
    }

    return QDomNode();
}

BacnetDeviceObject *ObjectsStructureFactory::createDevice(QDomElement &deviceElement)
{
    bool ok(true);
    //get address
    BacnetAddress address = BacnetInternalAddressHelper::toBacnetAddress(deviceElement.attribute(DeviceInternalAddressAttribute).toUInt(&ok));
    if (!ok || !address.isAddrInitialized() ) {
        qDebug("%s : Address can't be created (%s) or already exists", __PRETTY_FUNCTION__, qPrintable(deviceElement.attribute(DeviceInternalAddressAttribute)));
        return 0;
    }

    //get device instance number
    quint32 instanceNum = deviceElement.attribute(DeviceInstanceNumberAttribute).toUInt(&ok);
    if (!ok) {
        qDebug("%s : Can't create instance number or already used %s", __PRETTY_FUNCTION__, qPrintable(deviceElement.attribute(DeviceInstanceNumberAttribute)));
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

void ObjectsStructureFactory::createObject(BacnetDeviceObject *toBeParentDevice, QDomElement &objectElement)
{
    //get device instance number
    bool ok;
    quint32 instanceNum = objectElement.attribute(ObjectInstanceNumberAttribute).toUInt(&ok);
    if (!ok ) {
        qDebug("%s : Can't get object instance number %s", __PRETTY_FUNCTION__, qPrintable(objectElement.attribute(ObjectInstanceNumberAttribute)));
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



void ObjectsStructureFactory::populateWithProperties(BacnetObject *object, QDomElement &propertiesRootElement)
{
    bool ok;
    for (QDomElement pElem = propertiesRootElement.firstChildElement(ObjectPropertyTagName); !pElem.isNull(); pElem = pElem.nextSiblingElement(ObjectPropertyTagName)) {
        //required attributes - property type and property identifier
        if ( !pElem.hasAttribute(PropertyPropertyTypeAttribute) ||
             !pElem.hasAttribute(PropertyIdentifierAttribute) ) {
            qDebug("%s : Property doesn't ahve required attributes.", __PRETTY_FUNCTION__);
            continue;
        }

        //get property identifier
        BacnetPropertyNS::Identifier propId = (BacnetPropertyNS::Identifier)(pElem.attribute(PropertyIdentifierAttribute).toUInt(&ok));
        if (!ok)
            continue;

        BacnetProperty *property = createAbstractProperty(pElem, object);
        if ((0 == property) || !object->addProperty(propId, property))
            delete property;
    }
}

BacnetProperty *ObjectsStructureFactory::createAbstractProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport)
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
            qDebug("%s : Property proxy doesn't have bacnet type attribute", __PRETTY_FUNCTION__);
            return 0;
        }
        return createInternalProxyProperty(propElem, containerSupport);

    }

    return 0;
}

BacnetProperty *ObjectsStructureFactory::createInternalProxyProperty(QDomElement &propElem, InternalPropertyContainerSupport *containerSupport)
{
    //get BacnetApp tag
    QMap<QString, ObjectsStructureFactory::TagConversion>::Iterator typeIt = _typesMap.find(propElem.attribute(PropertyBacnetTypeAttribute));
    if (typeIt == _typesMap.end()) {
        qDebug("%s : bac property unknown", __PRETTY_FUNCTION__);
        return 0;
    }
    ::Property *intenralProperty = _dataModel->createProperty(propElem);
    if (0 == intenralProperty)
        return 0;

    return new ProxyInternalProperty(intenralProperty, typeIt->bacnetType, intenralProperty->type(), containerSupport);
}

BacnetProperty *ObjectsStructureFactory::createSimpleProperty(QDomElement &propElem)
{
    bool ok;

    //get BacnetApp tag
    QMap<QString, ObjectsStructureFactory::TagConversion>::Iterator typeIt = _typesMap.find(propElem.attribute(PropertyBacnetTypeAttribute));
    if (typeIt == _typesMap.end()) {
        qDebug("%s : bac property unknown", __PRETTY_FUNCTION__);
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

QMap<QString, BacnetObjectTypeNS::ObjectType> ObjectsStructureFactory::supportedObjectsTypes()
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

QMap<QString, ObjectsStructureFactory::TagConversion> ObjectsStructureFactory::typesMap()
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

ObjectsStructureFactory *ObjectsStructureFactory::_instance = 0;
ObjectsStructureFactory *ObjectsStructureFactory::instance()
{
    if (0 == _instance)
        _instance = new ObjectsStructureFactory();
    return _instance;
}

ObjectsStructureFactory::ObjectsStructureFactory():
    _typesMap(typesMap()),
    _supportedObjectsTypes(supportedObjectsTypes())
{

}

void ObjectsStructureFactory::releaseInstance()
{
    delete _instance;
    _instance = 0;
}

QVariant ObjectsStructureFactory::qvariantFromValue(QString value, TagConversion types, bool *doneOk)
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
