#ifndef CDM_H
#define CDM_H

#include <QMap>
#include "property.h"

class DataModel
{
public:
    static DataModel *instance();

    /**Creates PropertySubject of propertyType and registers it at propId id. If the id is already reserverd
      returns 0 pointer.
      */
    PropertySubject *createProperty(quint32 propId, QVariant::Type propertyType);

    PropertySubject *getProperty(quint32 propId);

    PropertyObserver *createPropertyObserver(quint32 propId);

private:
    DataModel();

private:
    static DataModel *_instance;
    QMap<quint32, PropertySubject*> _properties;
};

#endif // CDM_H
