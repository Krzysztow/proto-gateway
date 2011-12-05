#ifndef SNGHANDLER_H
#define SNGHANDLER_H

#include <QHostAddress>
#include <QDomElement>

class PropertyOwner;

namespace Sng {

class SngHandler
{
public:
    SngHandler();

public:
    void addPropertyMapping(::PropertyOwner *owner);

private:
    QList<PropertyOwner*> _propertiesMappings;

};

}
#endif // SNGHANDLER_H
