#ifndef ASYNCHOWNER_H
#define ASYNCHOWNER_H

#include <QObject>
#include <QTimer>
#include <QList>


#include "propertyowner.h"

class PropertySubject;
class AsynchOwner:
        public QObject,
        public PropertyOwner
{
    Q_OBJECT;
public:
    AsynchOwner(QObject *parent = 0);

    ~AsynchOwner();

public://overriden from PropertyOwner
    void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    int getPropertyRequested(PropertySubject *toBeGotten);
    int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);
    void propertyValueChanged(Property *property);

    void addProperty(Property *property);

private slots:
    void timeout();
    void setExtValue();

private:
    QTimer *_timer;
    QList<Property *> _properties;
    QList<AsynchData> _asynchReqs;
    PropertyObserver *tempProp;
};

#endif // ASYNCHOWNER_H
