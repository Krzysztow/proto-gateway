#ifndef PROPERTY_H
#define PROPERTY_H

#include <QList>
#include <QVariant>


class PropertySubject;
class PropertyObserver;
class PropertyOwner;

class Property
{
public:
    enum ActiontResult {
        UnknownError    = -7,
        UnknownProperty = -6,
        InternalTimeout = -5,
        Timeout         = -4,
        IvalidRange     = -3,
        TypeMismatch    = -2,
        NotSetYet       = -1,

        ResultOk        =  0
    };

public:
    Property();
    virtual ~Property();

    /**
      If positive, then this is a unique schedule number of asynchronous call.
      Otherwise, either data is read and is OK, or data not set yet or other error.
      */
    virtual int getValue(QVariant *outValue) = 0;

    /**
      Should be used only then, when we know that property has the newest/vaild value. Most probably
      it should be used by PropertyOwner when the asynch action is finished.
      */
    virtual int getValueInstant(QVariant *outValue) = 0;

    /** Is meant to be used in order to set the value of the property.
      \param settingObserver is passed so that when value is changed, it is not infomred back.
      */
    virtual int setValue(QVariant &inValue) = 0;

    /**
      \sa PropertySubject::asynchSetValueResponse() & PropertyObserver::asynchSetValueResponse() for information on specific usage.

      \note Why this has to be a function of this interface? And why not to pass PropertyOwner - this one is informed at the end.
      The problem is we should (should we?) have information about the property proxy, that was used so that we don't inform it about
      the change - if it was, then doubled reaction to asynchronous callbacks would be called.
      */
    virtual void asynchActionFinished(int asynchId, Property::ActiontResult actionResult) = 0;

    //! Is meant to return the type of the property.
    virtual QVariant::Type type() = 0;

    virtual void setOwner(PropertyOwner *owner);
    virtual PropertyOwner *owner();
};

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

private:
    /** These two methods should be used only by the PropertyOwner. They do the same as other set/getValue methods, but don't check
        for the owner to approve the request.
      */
    void setValueInstantly(QVariant &inValue, PropertyObserver *observerToOmit);

    QList<PropertyObserver *>_observers;
    QVariant _value;
    PropertyOwner *_owner;
};

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

private:
    PropertySubject *_property;
    PropertyOwner *_owner;
};

class PropertyUnshared:
        public Property
{
    PropertyUnshared(QVariant::Type propType);

    /** Overrides and implements Property::getValue().
        \param outValue is set to internal value.
        \returns always returns Property::Ready.
     */
    virtual int getValueSafe(QVariant *outValue);

    /** Overrides and implements Property::getValue().
        \param inValue is set to internal value, if the type is consistent with internal one.
        \returns always returns Property::Ready if ok, or some error if, for instance, there was a type mismatch.
      */
    virtual int setValueSafe(QVariant &inValue);

    //! Is meant to return the type of the property.
    QVariant::Type type();

private:
    QVariant _value;
};

//todo create unshared property

#endif // PROPERTY_H
