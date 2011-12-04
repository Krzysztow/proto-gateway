#ifndef PROPERTYOBSERVER_H
#define PROPERTYOBSERVER_H

#include "property.h"

class PropertyObserver:
        public Property
{
public://functions that override Property virtuals

    //! Property::getValue() implemented - internally it asks the original property to be read \sa PropertySubject::getValueSafe().
    virtual int getValue(QVariant *outValue);

    /**
      This function should be called by the PropertySubject if:
      - this instance, having requested PropertySubject::getValue() got asynchronous code as a response;
      - there is an answer (be it success true or false) to the asynchronous above query.
      */
    virtual void asynchActionFinished(int asynchId, Property::ActiontResult actionResult);

    //! Property::setValue() implementation - internally asks original property to be set \sa PropertySubject::setValueSafe()
    virtual int setValue(QVariant &inValue);

    virtual void setOwner(PropertyOwner *owner);
    virtual PropertyOwner *owner();

    /**
      Should be used only then, when we know that property has the newest/vaild value. Most probably
      it should be used by PropertyOwner when the asynch action is finished.
      */
    int getValueInstant(QVariant *outValue);

public://hooks for PropertyObserver descendants
    /** This is a hook function, that is called whenever value of the property is being changed.
      */
    virtual void propertyValueChanged();

public://functions from PropertyObserver itself
    PropertyObserver(PropertyOwner *container, PropertySubject *property);
    virtual ~PropertyObserver();

    //! Property::type() implamented
    QVariant::Type type();

protected:
    PropertySubject *_property;

private:
    PropertyOwner *_owner;
};


#endif // PROPERTYOBSERVER_H
