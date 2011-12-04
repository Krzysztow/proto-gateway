#ifndef SNG_SNGCOMMON_H
#define SNG_SNGCOMMON_H

#include <QtCore>
#include "connectionframe.h"

namespace Sng {

class Sngcommon
{
public:
    Sngcommon();

    QVariant::Type variantTypeForSngType(ConnectionFrame::DataType type);
};

} // namespace Sng

#endif // SNG_SNGCOMMON_H
