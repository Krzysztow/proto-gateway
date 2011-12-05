#include "sngfactory.h"

#include "snginternalproperty.h"
#include "sngexternalproperty.h"
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
static const char SngPropertyTypeAttribute[]        = "prop-type";
static const char SngPropTypeInternalValue[]        = "internal";
static const char SngPropTypeExternalValue[]        = "external";

SngHandler *SngFactory::createModule(QDomElement &sngConfig)
{
    SngHandler *handler = new SngHandler();

    GroupAddress address;
    bool ok;
    for (QDomElement propertyElem = sngConfig.firstChildElement(SngPropertyDefinitionTagName); !propertyElem.isNull(); propertyElem = propertyElem.nextSiblingElement(SngPropertyDefinitionTagName)) {
        qDebug()<<"Got element"<<ConfiguratorHelper::elementString(propertyElem);
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
        if (SngPropTypeInternalValue == str) {
            PropertyObserver *property = DataModel::instance()->createPropertyObserver(propertyElem);
            if (0 == property) {
                ConfiguratorHelper::elementError(propertyElem, "", "Observer mapping not created!");
                continue;
            }
            createdOwner = new Sng::SngInternalProperty(property, type, address);
        } else if (SngPropTypeExternalValue == str) {
            PropertySubject *property = DataModel::instance()->createPropertySubject(propertyElem);
            if (0 == property) {
                ConfiguratorHelper::elementError(propertyElem, "", "Subject not created!");
                continue;
            }
            createdOwner = new Sng::SngExternalProperty(property, type, address);
        } else {
            ConfiguratorHelper::elementError(propertyElem, SngPropertyTypeAttribute);
            continue;
        }

        if (0 != createdOwner)
            handler->addPropertyMapping(createdOwner);
    }

    return handler;
}
