#include "snginternalsupport.h"

using namespace Sng;

#define RETURN_IF_WRONG_TYPE(expected, gotten)                      \
    if (expected !=  gotten) {                                      \
        Inform_Wrong_Type(expected, gotten);                        \
        return;                                                     \
    }

#ifdef DEBUG_TYPE
#define Inform_Wrong_Type(a, b) informWrongType(a, b);
void SngInternalSupport::informWrongType(ConnectionFrame::DataType typeExpected, ConnectionFrame::DataType typeGotten)
{
    qDebug("%s : received %d, whereas am looking only for %d type", __PRETTY_FUNCTION__, typeGotten, typeExpected);
}
#else
#define Inform_Wrong_Type(a, b) ;
#endif


SngInternalSupport::SngInternalSupport(GroupAddress rxAddress, ConnectionFrame::DataType rxType,
                                       GroupAddress txAddress, ConnectionFrame::DataType txType):
    _rxAddress(rxAddress),
    _rxType(rxType),
    _txAddress(txAddress),
    _txType(txType)
{
    if (_rxAddress.isValid()) {
        registerForAddress(_rxAddress);
    }
}

void SngInternalSupport::receiveOnOff(const GroupAddress &address, bool value)
{
    RETURN_IF_WRONG_TYPE(_rxType, ConnectionFrame::OnOff);

    Q_ASSERT(address == _rxAddress);
    QVariant varValue;
    varValue.setValue((bool)value);
    frameWithVariantReceived_hook(varValue);
}

void SngInternalSupport::receiveDimm(const GroupAddress &address, ObjectConnectionSupport::DimmCommand value)
{
    RETURN_IF_WRONG_TYPE(_rxType, ConnectionFrame::Dimm);

     Q_ASSERT(address == _rxAddress);
     QVariant varValue;
     varValue.setValue((uint)value);
     frameWithVariantReceived_hook(varValue);
}

void SngInternalSupport::receiveTirme(const GroupAddress &address, const QTime &value)
{
    RETURN_IF_WRONG_TYPE(_rxType, ConnectionFrame::Time);

    Q_ASSERT(address == _rxAddress);
    QVariant varValue(value);
    frameWithVariantReceived_hook(varValue);
}

void SngInternalSupport::receiveDate(const GroupAddress &address, const QDate &value)
{
    RETURN_IF_WRONG_TYPE(_rxType, ConnectionFrame::Date);

    Q_ASSERT(address == _rxAddress);
    QVariant varValue(value);
    frameWithVariantReceived_hook(varValue);
}

void SngInternalSupport::receiveTemp(const GroupAddress &address, float value)
{
    RETURN_IF_WRONG_TYPE(_rxType, ConnectionFrame::Temp);

    Q_ASSERT(address == _rxAddress);
    QVariant varValue(QMetaType::Float);
    varValue.setValue(value);
    frameWithVariantReceived_hook(varValue);
}

void SngInternalSupport::receiveValue(const GroupAddress &address, int value)
{
    RETURN_IF_WRONG_TYPE(_rxType, ConnectionFrame::Value);

     Q_ASSERT(address == _rxAddress);
     QVariant varValue;
     varValue.setValue((uint)value);

     frameWithVariantReceived_hook(varValue);
}

bool SngInternalSupport::sendFrameWithVariant_helper(ConnectionFrame::DataType addressType, GroupAddress &address, QVariant &value)
{
    bool ok(false);
    switch (addressType) {
    case (ConnectionFrame::OnOff): {
        sendOnOff(address, value.toBool());
        ok = true;
    }
        break;
    case (ConnectionFrame::Dimm): {
        DimmCommand v = (DimmCommand)value.toUInt(&ok);
        if (ok)
            sendDimm(address, v);
    }
        break;
    case (ConnectionFrame::Time):{
        QTime v = value.toTime();
        if (v.isValid()) {
            sendTime(address, v);
            ok = true;
        }
    }
        break;
    case (ConnectionFrame::Date): {
        QDate v = value.toDate();
        if (v.isValid()) {
            sendDate(address, v);
            ok = true;
        }
    }
        break;
    case (ConnectionFrame::Temp): {
        float v = value.toFloat(&ok);
        if (ok)
            sendTemp(address, v);
    }
        break;
    case (ConnectionFrame::Value): {
        quint8 v = value.toUInt(&ok);
        if (ok)
            sendValue(address, v);
    }
        break;
    default:
        Q_ASSERT(false);
        //    case (ConnectionFrame::Float4B):
        //        break;
        //    case (ConnectionFrame::Value2B):
        //        break;
        //    case (ConnectionFrame::Value4B):
        //        break;
        //    case (ConnectionFrame::Value_Signed):
        //        break;
        //    case (ConnectionFrame::Value2B_Signed):
        //        break;
        //    case (ConnectionFrame::Value4B_Signed):
        //        break;
    }

    return ok;
}
