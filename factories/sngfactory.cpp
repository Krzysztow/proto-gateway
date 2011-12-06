#include "sngfactory.h"

#include "sngsimplesensorproperty.h"
#include "sngsimpleactorproperty.h"
#include "cdm.h"
#include "propertysubject.h"
#include "propertyobserver.h"
#include "propertyconverter.h"
#include "propertywithconversionobserver.h"
#include "configuratorhelper.h"
#include "sngdefinitions.h"
#include "snghandler.h"
#include "connectionmanager.h"

using namespace Sng;

static const char SngPropertyDefinitionTagName[]    = "property";
static const char SngTypeAttribute[]                = "sng-type";
static const char SngAddressAttribute[]             = "gr-address";
static const char SngFBackAddressAttribute[]        = "gr-address-fback";
static const char SngPropertyTypeAttribute[]        = "prop-type";
static const char SngPropTypeSimpleSensorValue[]    = "simple-sensor";
static const char SngPropTypeSimpleActorValue[]     = "simple-actor";



SngHandler *SngFactory::createModule(QDomElement &sngConfig)
{
    SngHandler *handler = new SngHandler();

    GroupAddress address;
    GroupAddress feedback;
    bool ok;
    for (QDomElement propertyElem = sngConfig.firstChildElement(SngPropertyDefinitionTagName); !propertyElem.isNull(); propertyElem = propertyElem.nextSiblingElement(SngPropertyDefinitionTagName)) {
        //get sng type
        ConnectionFrame::DataType type = SngDefinitions::typeFromString(propertyElem.attribute(SngTypeAttribute), ok);
        if (0 == type) {
            ConfiguratorHelper::elementError(propertyElem, SngTypeAttribute);
            continue;
        }

        //get sng address
        QString str = propertyElem.attribute(SngAddressAttribute);
        ok |= address.fromString(str);
        if (!ok) {
            ConfiguratorHelper::elementError(propertyElem, "", qPrintable(QString("Wrong &1 or %2").arg(SngTypeAttribute).arg(SngAddressAttribute)));
            continue;
        }

        //get feedback address, it's not obligatory, so don't care if it doesn't work
        str = propertyElem.attribute(SngFBackAddressAttribute);
        feedback.fromString(str);

        //if all is fine, we can create a property.
        str = propertyElem.attribute(SngPropertyTypeAttribute);
        ::PropertyOwner *createdOwner(0);
        if (SngPropTypeSimpleSensorValue == str) {
            PropertyObserver *property = DataModel::instance()->createPropertyObserver(propertyElem);
            if (0 == property) {
                ConfiguratorHelper::elementError(propertyElem, "", "Observer mapping not created!");
                continue;
            }
            createdOwner = new Sng::SngSimpleSensorProperty(property, type, address, type, feedback);
        } else if (SngPropTypeSimpleActorValue == str) {
            PropertySubject *property = DataModel::instance()->createPropertySubject(propertyElem);
            if (0 == property) {
                ConfiguratorHelper::elementError(propertyElem, "", "Subject not created!");
                continue;
            }
            createdOwner = new Sng::SngSimpleActorProperty(property, type, address, type, address);
        } else {
            ConfiguratorHelper::elementError(propertyElem, SngPropertyTypeAttribute);
            continue;
        }

        if (0 != createdOwner)
            handler->addPropertyMapping(createdOwner);
    }

    return handler;
}
