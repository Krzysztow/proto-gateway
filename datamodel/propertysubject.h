#ifndef PROPERTYSUBJECT_H
#define PROPERTYSUBJECT_H

#include "property.h"

class DataModel;
class PropertySubject:
        public Property
{
public:
    PropertySubject(PropertyOwner *container, QVariant::Type propType);

    //! Gets the value without any further checks. Always returns Property::Ready.
    virtual int getValue(QVariant *outValue);

    //! In this case does the same as PropertySubject::getValue() one.
    virtual int getValueInstant(QVariant *outValue);

    /** This function is going to be called by the owner, when the asynchronous response to the previous
        get request is received.
        Internally, this function invokes asynchGetValueResponse() in all its observers.
      */
    virtual void asynchActionFinished(int asynchId, Property::ActiontResult actionResult);

    //! Sets the value without any other checks and returns Property::Ready. After it's done, informs all the observers that value has changed.
    virtual int setValue(QVariant &inValue);

    //! Property::type() overriden - internally it reads the type of stored value
    virtual QVariant::Type type();

    virtual void setOwner(PropertyOwner *owner);
    virtual PropertyOwner *owner();

public:
    /** Property::getValue() implemented - internally it asks its container if the value can be distributed.
        If so, then returns Property::Ready value. If not, returns either error number or asynchronous ID, that
        is to be called when container read operation is done. This function should be called by PropertyObservers.
     */
    int getValueSafe(QVariant *outValue, PropertyObserver *requester);

    /** This method should be called whenever the setter is an instance of PropertyObserver. Otherwise, the PropertyObserver::propertyChanged()
       is called and most probaably you want to avoid that.
      */
    int setValueSafe(QVariant &inValue, PropertyObserver *requester);

    //! Registers PropertyObserver. From now on, whenever property value is changed, the observer will be informed.
    void addObserver(PropertyObserver *observer);

    //! Unregisteres observer.
    void rmObserver(PropertyObserver *observer);

    //! Set value without informing anyobserver.
    void setValueSilent(QVariant &inValue);

public:
    //! Returns true, if not initilialized (value not set).
    bool isNull() {return _value.isNull();}

private:
    /** These two methods should be used only by the PropertyOwner. They do the same as other set/getValue methods, but don't check
        for the owner to approve the request.
      */
    void setValueInstantly(QVariant &inValue, PropertyObserver *observerToOmit);

    /** This method is not to be called by anyone else but DataModel. It's used for setting types of PropertySubject when its watchers
      were already taken.
      */
    friend class DataModel;
    void setType(QVariant::Type propType);

    QList<PropertyObserver *>_observers;
    QVariant _value;
    PropertyOwner *_owner;
};


#endif // PROPERTYSUBJECT_H
