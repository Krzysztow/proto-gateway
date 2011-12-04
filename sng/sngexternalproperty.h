#ifndef SNG_SNGEXTERNALPROPERTY_H
#define SNG_SNGEXTERNALPROPERTY_H

#include "propertyowner.h"
#include "objectconnectionsupport.h"
#include "connectionframe.h"

namespace Sng {

class SngExternalProperty:
        public PropertyOwner,
        public ObjectConnectionSupport
{
public:
    SngExternalProperty();

protected://methods overridden from PropertyOwner - none is implemented
    //! In case of this class, we don't care about this hook method. In other, that implement acknowledging, one has to override it.
    virtual int getPropertyRequested(PropertySubject *toBeGotten);
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);

    //! This method should never be called. Property is going to be only observer.
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    //! This method should never be called. Property is going to be only observer.
    virtual void propertyValueChanged(Property *property);

    //this class DOES NOT override any functions by ObjectConnectionSupport. It just gets value and sends to address.

protected:
    //! Sends frame with group address _extAddressType with value taken instantly from _property.
    void setExternalProperty_helper();

protected:
    Property *_property;

    ConnectionFrame::DataType _extAddressType;
    GroupAddress _extAddressToCommand;
};

} // namespace Sng

#endif // SNG_SNGEXTERNALPROPERTY_H
