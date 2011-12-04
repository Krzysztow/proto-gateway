#include "sngexternalproperty.h"

#include "sngcommon.h"
#include "propertysubject.h"

using namespace Sng;

SngExternalProperty::SngExternalProperty()
{
}

void SngExternalProperty::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_UNUSED(asynchId);
    Q_UNUSED(property);
    Q_UNUSED(actionResult);
    Q_CHECK_PTR(_property);
    Q_ASSERT(false);
}

void SngExternalProperty::propertyValueChanged(Property *property)
{
    Q_UNUSED(property);
}

int SngExternalProperty::getPropertyRequested(PropertySubject *toBeGotten)
{
    Q_UNUSED(toBeGotten);
    Q_ASSERT(_property == toBeGotten);

    /** \todo we can't read our properties. The only one thing is that we could introduce a flag indicating if value has been
        received at least once.
      */
        if (toBeGotten->isNull())
            return Property::NotSetYet;
         else
            return Property::ResultOk;
}

int SngExternalProperty::setPropertyRequested(PropertySubject *toBeSet, QVariant &value)
{
    Q_ASSERT(toBeSet == _property);
    if (toBeSet != _property)
        return Property::UnknownProperty;

    if (!_extAddressToCommand.isValid()) {
        qDebug("%s : Adderess not valid", __PRETTY_FUNCTION__);
        return Property::UnknownError;
    }

    bool ok(false);
    switch (_extAddressType) {
    case (ConnectionFrame::OnOff): {
        sendOnOff(_extAddressToCommand, value.toBool());
        ok = true;
    }
        break;
    case (ConnectionFrame::Dimm): {
        DimmCommand v = (DimmCommand)value.toUInt(&ok);
        if (ok)
            sendDimm(_extAddressToCommand, v);
    }
        break;
    case (ConnectionFrame::Time):{
        QTime v = value.toTime();
        if (v.isValid()) {
            sendTime(_extAddressToCommand, v);
            ok = true;
        }
    }
        break;
    case (ConnectionFrame::Date): {
        QDate v = value.toDate();
        if (v.isValid()) {
            sendDate(_extAddressToCommand, v);
            ok = true;
        }
    }
        break;
    case (ConnectionFrame::Temp): {
        float v = value.toFloat(&ok);
        if (ok)
            sendTemp(_extAddressToCommand, v);
    }
        break;
    case (ConnectionFrame::Value): {
        quint8 v = value.toUInt(&ok);
        if (ok)
            sendValue(_extAddressToCommand, v);
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

    if (ok)
        return Property::ResultOk;
    else
        return Property::UnknownError;
}
