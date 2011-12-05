#ifndef SNGDEFINITIONS_H
#define SNGDEFINITIONS_H

#include <QtGlobal>
#include "connectionframe.h"

namespace SngDefinitions
{
    static const int ConnFrameDataFieldsLength = 4;

    enum DimmValues {
        StartUp   = ConnectionFrame::StartUp,
        StopUp    = ConnectionFrame::StopUp,
        StartDown = ConnectionFrame::StartDown,
        StopDown  = ConnectionFrame::StopDown
                };

    QString typeToString(const ConnectionFrame& frame);
    QString typeToString(quint8 type);
    ConnectionFrame::DataType typeFromString(const QString &strType, bool &ok);
    QString valueToString(const ConnectionFrame& frame);
    QString valueToString(const quint8 *value, ConnectionFrame::DataType type);
    bool setValueFromString(const QString &type, const QString &value, ConnectionFrame &frame);
    bool valueFromString(const QString &type, const QString &value, quint8 data[ConnFrameDataFieldsLength]);
}
#endif // SNGDEFINITIONS_H
