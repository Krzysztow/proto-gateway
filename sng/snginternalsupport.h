#ifndef SNG_SNGINTERNALSUPPORT_H
#define SNG_SNGINTERNALSUPPORT_H

#include "propertyowner.h"
#include "objectconnectionsupport.h"
#include "connectionframe.h"

#define DEBUG_TYPE

namespace Sng {

class SngInternalSupport:
        public ObjectConnectionSupport
{
public:
    SngInternalSupport(GroupAddress rxAddress, ConnectionFrame::DataType rxType,
                       GroupAddress txAddress, ConnectionFrame::DataType txType);

protected://overridden from ObjectConnectionSupport
    virtual void receiveOnOff(const GroupAddress &address, bool value);
    virtual void receiveDimm(const GroupAddress &address, DimmCommand value);
    virtual void receiveTirme(const GroupAddress &address, const QTime& value);
    virtual void receiveDate(const GroupAddress &address, const QDate& value);
    virtual void receiveTemp(const GroupAddress &address, float value);
    virtual void receiveValue(const GroupAddress &address, int value);

protected:
    /** This is the hook, that most probably you want to override when inheriting from this class, because it's the hook called
      when a frame with address _rxAddres and type _rxType is received. The value is translated (based on frame type) into its internal
      QVariant representation and then this hook is called.
      */
    virtual void frameWithVariantReceived_hook(QVariant &propertyValue) = 0;

    /** Sends frame with group address address and value value converted to type addressType. Returns true on success.
      Of course you can use any address, but most probably it will be _txAddress of _txType type.
      */
    bool sendFrameWithVariant_helper(ConnectionFrame::DataType addressType, GroupAddress &address, QVariant &value);

private:
#ifdef DEBUG_TYPE
    void informWrongType(ConnectionFrame::DataType typeExpected, ConnectionFrame::DataType typeGotten);
#endif


protected:
    GroupAddress _rxAddress;
    ConnectionFrame::DataType _rxType;
    //! \todo Decide if following vars are to be here. No processing is done on these two values.
    GroupAddress _txAddress;
    ConnectionFrame::DataType _txType;
};

} // namespace Sng

#endif // SNG_SNGINTERNALSUPPORT_H
