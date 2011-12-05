#ifndef SNG_SNGINTERNALTYPESMAPPER_H
#define SNG_SNGINTERNALTYPESMAPPER_H

#include "connectionframe.h"

namespace SngInternalTypesMapper {

//ConnectionFrame::DataType internalTypeToSng(QVariant::Type type);
QVariant::Type sngTypeToInternal(ConnectionFrame::DataType type);

} // namespace Sng

#endif // SNG_SNGINTERNALTYPESMAPPER_H
