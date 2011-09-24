#ifndef PROPERTYOWNER_H
#define PROPERTYOWNER_H

#include <QVariant>

#include "property.h"
/**
  The property container is meant to be inherited by a protocol. This would decide if the property
  is ready to be read/written or some asynchronous steps are to be taken.
  */

class PropertyOwner
{
public:
    enum RequestType {
        RequestGet,
        RequestSet
    };

    struct AsynchData {
        PropertySubject *prop;
        int id;
        RequestType reqType;
        QVariant valSet;
    };

public:
    virtual ~PropertyOwner();
    /**
      This is a hook function, being used by the property, which is to be read. Property owner decides whether
      the value of the property is ready to be read straigh away or not. In case of yes,
      this function should return Ready. In case of not, if the container can arrange something asynchronously to obtain the value
      of the property, do so and return unique id gotten from CDM. Otherwise return NotSetYet.
      \param toBeGotten is a poitner to the property that is to be read;
      \param requester - pointer to the requester. This pointer should be remembered and passed back, when asynch response is
             generated with Property::asynchGetResponse(). Having this pointer avoids sending too many information about property value
             to the requester.
      */
    virtual int getPropertyRequest(PropertySubject *toBeGotten) = 0;

    /**
      Like in a case of reading property. This function is meant to check if the value is in the correct range,
      or to request asynchronous operations to set the real object to that value.
      */
    virtual int setPropertyRequest(PropertySubject *toBeSet, QVariant &value) = 0;

    /** Hook method, invoked when the asynchronous get action on property is finished. This should be invoked.
      */
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult) = 0;

    /** Hoo method - invoked whenever PropertySubject being observed by owned PropertyObserver instance is changed.
      */
    virtual void propertyValueChanged(PropertyObserver *property) = 0;
};

#endif // PROPERTYOWNER_H
