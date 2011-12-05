#include "snginternaltypesmapper.h"

//ConnectionFrame::DataType SngInternalTypesMapper::internalTypeToSng(QVariant::Type type)
//{
//    switch (type) {

//    }
//}


QVariant::Type SngInternalTypesMapper::sngTypeToInternal(ConnectionFrame::DataType type)
{
    switch (type) {
    case (ConnectionFrame::OnOff):
        return QVariant::Bool;
    case (ConnectionFrame::Dimm):
        return (QVariant::Type)QMetaType::UChar;
    case (ConnectionFrame::Time):
        return QVariant::Time;
    case (ConnectionFrame::Date):
        return QVariant::Date;
    case (ConnectionFrame::Temp):
        return (QVariant::Type)QMetaType::Float;
    case (ConnectionFrame::Value):
        return (QVariant::Type)::QMetaType::UChar;
    default:
        Q_ASSERT(false);
        return QVariant::Invalid;
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

}
