#ifndef SNG_SNGSIMPLESENSORPROPERTY_H
#define SNG_SNGSIMPLESENSORPROPERTY_H

#include "propertyowner.h"
#include "snginternalsupport.h"
namespace Sng {

/** This class alows internal objects to be controlled by SNG Frames. The class is meant to wait on a frame with specific address
  \sa _setInternalCommand address, of specific type. When such frame is gotten, it orderes property to set it's value and don't care
  about the result. It also doesn't care, when value changes. If you wan't to send feedback, when value is set (attempted to be set) or
  cahnges, see SngSimpleAckdInternalProperty and SngComplexAckdInternalProperty.
  */
class SngSimpleSensorProperty:
        public PropertyOwner,
        public SngInternalSupport
{
public:
    SngSimpleSensorProperty(PropertyObserver *observer,
                            ConnectionFrame::DataType rxAddrType, GroupAddress &rxAddress,
                            ConnectionFrame::DataType txAddrType, GroupAddress &txAddress);

protected://methods overridden from PropertyOwner
    //! This method should never be called. Property is going to be only observer.
    virtual int getPropertyRequested(PropertySubject *toBeGotten);
    //! This method should never be called. Property is going to be only observer.
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);

    //! In case of this class, we don't care about this hook method. In other, that implement acknowledging, one has to override it.
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    virtual void propertyValueChanged(Property *property);

protected://methods overridden from SngInternalSupport.
    virtual void frameWithVariantReceived_hook(QVariant &propertyValue);

protected:
    PropertyObserver *_property;

};

} // namespace Sng

#endif // SNG_SNGSIMPLESENSORPROPERTY_H
