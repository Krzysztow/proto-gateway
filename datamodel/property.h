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

public:
    int generateAsynchId();
    void releaseId(int asynchId);
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
