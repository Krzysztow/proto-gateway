#include "snghandler.h"
#include "connectionmanager.h"

using namespace Sng;

SngHandler::SngHandler()
{
}

void SngHandler::addPropertyMapping(::PropertyOwner *owner)
{
    Q_ASSERT(!_propertiesMappings.contains(owner));
    _propertiesMappings.append(owner);
}

//#define SNGHANDLER_TEST
#ifdef SNGHANDLER_TEST

#include <QFile>
#include <QDomElement>
#include <QDebug>

#include "sngsimplesensorproperty.h"
#include "sngsimpleactorproperty.h"
#include "cdm.h"
#include "sngasynchvaluesetter.h"
#include "propertysubject.h"
#include "propertyobserver.h"
#include "propertyconverter.h"
#include "propertywithconversionobserver.h"
#include "configuratorhelper.h"
#include "sngdefinitions.h"
#include "sngsimplesensorproperty.h"
#include "sngsimpleactorproperty.h"

static const char SngPropertiesTagName[]            = "sngProperties";
static const char SngPropertyDefinitionTagName[]    = "property";
static const char SngTypeAttribute[]                = "sng-type";
static const char SngAddressAttribute[]             = "gr-address";
static const char SngPropertyTypeAttribute[]        = "prop-type";
static const char SngPropTypeSimpleSensorValue[]    = "simple-sensor";
static const char SngPropTypeSimpleActorValue[]     = "simple-actor";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile f("sng-test-config.xml");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Can't open the configuration file.");
        return 1;
    }

    QDomDocument doc;
    if (!doc.setContent(&f)) {
        qDebug("Malformed sng config!");
        return 2;
    }


    DataModel::instance()->startFactory();


    SngHandler *handler = new SngHandler();
    handler->setConfig(doc);

    QDomElement el = doc.documentElement().firstChildElement(SngPropertiesTagName);
    GroupAddress address;
    bool ok;
    for (QDomElement propertyElem = el.firstChildElement(SngPropertyDefinitionTagName); !propertyElem.isNull(); propertyElem = propertyElem.nextSiblingElement(SngPropertyDefinitionTagName)) {
        //get sng type
        ConnectionFrame::DataType type = SngDefinitions::typeFromString(propertyElem.attribute(SngTypeAttribute), ok);
        //get sng address
        QString str = propertyElem.attribute(SngAddressAttribute);
        ok |= address.fromString(str);
        if (!ok) {
            ConfiguratorHelper::elementError(propertyElem, "", qPrintable(QString("Wrong &1 or %2").arg(SngTypeAttribute).arg(SngAddressAttribute)));
            continue;
        }

        //if all is fine, we can create a property.
        str = propertyElem.attribute(SngPropertyTypeAttribute);
        ::PropertyOwner *createdOwner(0);
        if (SngPropTypeSimpleSensorValue == str) {
            PropertyObserver *property = DataModel::instance()->createPropertyObserver(propertyElem);
            if (0 == property) {
                ConfiguratorHelper::elementError(propertyElem, "", "Observer mapping not created!");
                continue;
            }
            createdOwner = new Sng::SngSimpleSensorProperty(property, type, address);
        } else if (SngPropTypeSimpleActorValue == str) {
            PropertySubject *property = DataModel::instance()->createPropertySubject(propertyElem);
            if (0 == property) {
                ConfiguratorHelper::elementError(propertyElem, "", "Subject not created!");
                continue;
            }
            createdOwner = new Sng::SngSimpleActorProperty(property, type, address);
        } else {
            ConfiguratorHelper::elementError(propertyElem, SngPropertyTypeAttribute);
            continue;
        }

        if (0 != createdOwner)
            handler->addPropertyMapping(createdOwner);
    }

    DataModel::instance()->stopFactory();

    return a.exec();
}


#undef SNGHANDLER_TEST
#endif //SNGHANDLER_TEST
