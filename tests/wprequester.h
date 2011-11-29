#ifndef WPREQUESTER_H
#define WPREQUESTER_H

#include <QObject>

#include "propertyowner.h"
#include "property.h"

class WpRequester:
        public QObject,
        public PropertyOwner
{
    Q_OBJECT
public:
    WpRequester(Property *propertyToWrite, QVariant valueToWrite);

protected:
    virtual int getPropertyRequested(PropertySubject *toBeGotten);
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    virtual void propertyValueChanged(Property *property);

public slots:
    void writeValue();

private:
    Property *_property;
    QVariant _value;
};

#endif // WPREQUESTER_H
