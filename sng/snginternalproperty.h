#ifndef SNG_SNGINTERNALPROPERTY_H
#define SNG_SNGINTERNALPROPERTY_H

#include "propertyowner.h"
#include "objectconnectionsupport.h"
#include "connectionframe.h"

#define DEBUG_TYPE

namespace Sng {


/** This class alows internal objects to be controlled by SNG Frames. The class is meant to wait on a frame with specific address
  \sa _setInternalCommand address, of specific type. When such frame is gotten, it orderes property to set it's value and don't care
  about the result. It also doesn't care, when value changes. If you wan't to send feedback, when value is set (attempted to be set) or
  cahnges, see SngSimpleAckdInternalProperty and SngComplexAckdInternalProperty.
  */
class SngInternalProperty:
        public PropertyOwner,
        public ObjectConnectionSupport
{
public:
    SngInternalProperty();

protected://methods overridden from PropertyOwner - none is implemented
    //! This method should never be called. Property is going to be only observer.
    virtual int getPropertyRequested(PropertySubject *toBeGotten);
    //! This method should never be called. Property is going to be only observer.
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);

    //! In case of this class, we don't care about this hook method. In other, that implement acknowledging, one has to override it.
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    virtual void propertyValueChanged(Property *property);

protected://overridden from ObjectConnectionSupport
    virtual void receiveOnOff(const GroupAddress &address, bool value);
    virtual void receiveDimm(const GroupAddress &address, DimmCommand value);
    virtual void receiveTirme(const GroupAddress &address, const QTime& value);
    virtual void receiveDate(const GroupAddress &address, const QDate& value);
    virtual void receiveTemp(const GroupAddress &address, float value);
    virtual void receiveValue(const GroupAddress &address, int value);

protected:
    /** This is the hook, that most probably you want to override when inheriting from SngInternalProperty to do some meaningful stuff and take care
        about result returned by Property::setValue().
        Here property is just set, and that's all. Use this if we don't care about acks.
      */
    virtual void setInternalProperty_hook(QVariant &propertyValue);

private:
#ifdef DEBUG_TYPE
    void informWrongType(ConnectionFrame::DataType typeExpected, ConnectionFrame::DataType typeGotten);
#endif

protected:
    PropertyObserver *_property;

    ConnectionFrame::DataType _setInternaCommandlType;
    GroupAddress _setInternalCommandAddress;
};

} // namespace Sng

#endif // SNG_SNGINTERNALPROPERTY_H
