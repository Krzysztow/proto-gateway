#ifndef SNG_SNGSIMPLEACTORPROPERTY_H
#define SNG_SNGSIMPLEACTORPROPERTY_H

#include "propertyowner.h"
#include "connectionframe.h"
#include "snginternalsupport.h"

/** This class is meant to represent simple actor in the SNG installation. This kind of actor may be set to some value,
    however it cannot set feedbacks - for instance Dimmer output that has no feedbacks set. Therefore we assume that
    received a SET frame (with set address) is also it's feedback, which means we should inform all the property observers
    about value change.
    So there are two operations:
    - when internal observer wants to set the value of Sng actuator, this class:
        # sends SET frame;
        # returns Property::ResultOk, so that DataModel informs all observers (excluding the requesting one) the value got changed
        # requeting observer gers ResultOk value, meaining request was successfull
    - when this instance receives a frame with a feedback address, it sets its property value, what results in all observers
    notification.
    \note The second case is exactly the one
  */

namespace Sng {

class SngSimpleActorProperty:
        public PropertyOwner,
        public SngInternalSupport
{
public:
    SngSimpleActorProperty(PropertySubject *subject, ConnectionFrame::DataType txAddrType, GroupAddress txAddress,
                           ConnectionFrame::DataType rxAddrType, GroupAddress &rxAddress);

protected://methods overridden from PropertyOwner - none is implemented
    //! In case of this class, we don't care about this hook method. In other, that implement acknowledging, one has to override it.
    virtual int getPropertyRequested(PropertySubject *toBeGotten);
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);

    //! This method should never be called. Property is going to be only observer.
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    //! This method should never be called. Property is going to be only observer.
    virtual void propertyValueChanged(Property *property);

protected://methods overriden from SngInternalSupport
    virtual void frameWithVariantReceived_hook(QVariant &propertyValue);

private:
    PropertySubject *_property;
};

} // namespace Sng

#endif // SNG_SNGSIMPLEACTORPROPERTY_H
