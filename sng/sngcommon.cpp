#include "sngcommon.h"


using namespace Sng;

Sngcommon::Sngcommon()
{

}

QVariant::Type Sngcommon::variantTypeForSngType(ConnectionFrame::DataType type)
{
    switch (type) {
    case (ConnectionFrame::OnOff):
        return (QVariant::Bool);
    case (ConnectionFrame::Time):
        return (QVariant::Time);
    case (ConnectionFrame::Date):
        return (QVariant::Date);
    case (ConnectionFrame::Temp):
    case (ConnectionFrame::Float4B):
        return (QVariant::Type)(QMetaType::Float);//the cast is safe - there is enough room in QVariant for Float
    case (ConnectionFrame::Dimm):
    case (ConnectionFrame::Value):              //fall through
    case (ConnectionFrame::Value2B):            //fall through
    case (ConnectionFrame::Value4B):
        return (QVariant::UInt);
    case (ConnectionFrame::Value_Signed):       //fall through
    case (ConnectionFrame::Value2B_Signed):     //fall through
    case (ConnectionFrame::Value4B_Signed):
        return (QVariant::Int);
    default:
        return (QVariant::Invalid);
    }
}
