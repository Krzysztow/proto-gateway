#include "sngdefinitions.h"

#include <QString>
#include <stdio.h>

#include "connectionframe.h"

QString SngDefinitions::typeToString(const ConnectionFrame& frame)
{
    return typeToString(frame.dataType());
}

QString SngDefinitions::typeToString(quint8 type)
{
    switch (type)
    {
    case (ConnectionFrame::OnOff):
        return "OnOff";
    case (ConnectionFrame::Value):
        {
            return "Value";
        }
    case (ConnectionFrame::Temp):
        {
            return "Temp";
        }
    case (ConnectionFrame::Dimm):
        {
            return "Dimm";
        }
    case (ConnectionFrame::Time):
        {
            return "Time";
        }
    case (ConnectionFrame::Date):
        {
            return "Date";
        }
    default:
        return "Unrecognized";
    }
}

ConnectionFrame::DataType SngDefinitions::typeFromString(const QString &strType, bool &ok)
{
    ok = true;
    QString str = strType.toLower();
    if (str.compare("onoff")==0) {
        return ConnectionFrame::OnOff;
    }
    else if (str.compare("dimm")==0) {
        return ConnectionFrame::Dimm;
    }
    else if (str.compare("time")==0) {
        return ConnectionFrame::Time;
    }
    else if (str.compare("date")==0) {
        return ConnectionFrame::Date;
    }
    else if (str.compare("temp")==0) {
        return ConnectionFrame::Temp;
    }
    else if (str.compare("value_unsigned")==0 ||
             str.compare("value")==0) {
        return ConnectionFrame::Value;
    }
    else if (str.compare("float4b")==0) {
        return ConnectionFrame::Float4B;
    }
    else if (str.compare("value2b_unsigned")==0) {
        return ConnectionFrame::Value2B;
    }
    else if (str.compare("value4b_unsigned")==0) {
        return ConnectionFrame::Value4B;
    }
    else if (str.compare("value_signed")==0) {
        return ConnectionFrame::Value_Signed;
    }
    else if (str.compare("value2b_signed")==0) {
        return ConnectionFrame::Value2B_Signed;
    }
    else if (str.compare("value4b_signed")==0) {
        return ConnectionFrame::Value4B_Signed;
    }

    ok = false;
    return (ConnectionFrame::DataType)0;
}

QString SngDefinitions::valueToString(const quint8 *value, ConnectionFrame::DataType type)
{
    ConnectionFrame frame;
    frame.setDataType(type);
    memcpy(frame.dataFields(), value, ConnFrameDataFieldsLength);
    return valueToString(frame);
}

QString SngDefinitions::valueToString(const ConnectionFrame& frame)
{
    switch (frame.dataType())
    {
    case (ConnectionFrame::OnOff):
        {
            return frame.onOff()?"On":"Off";
        }
    case (ConnectionFrame::Value):
        {
            return QString::number(frame.value());
        }
    case (ConnectionFrame::Temp):
        {
            return QString::number(frame.temp(), 'f', 2);
        }
    case (ConnectionFrame::Dimm):
        {
            switch (frame.data()[0])
            {
            case (ConnectionFrame::StartUp):
                return "StartUp";
            case (ConnectionFrame::StopUp):
                return "StopUp";
            case (ConnectionFrame::StartDown):
                return "StartDown";
            case (ConnectionFrame::StopDown):
                return "StopDown";
            }
        }
    case (ConnectionFrame::Time):
        {
            return frame.time().toString("hh:mm:ss");
        }
    case (ConnectionFrame::Date):
        {
            return frame.date().toString("yyyy-MM-dd");
        }
    default:
        return "-";
    }
}

bool SngDefinitions::setValueFromString(const QString &type, const QString &value, ConnectionFrame &frame)
{
    QString v = value.toLower();
    QString t = type.toLower();
    if (t.compare("onoff")==0)//frame is of OnOff type
    {
        bool val = ((v=="false" || v=="off")?false:true);
        frame.setOnOff(val);
    }
    else if (t.compare("value")==0)//Value
    {
        bool ok;
        quint8 val = v.toInt(&ok);
        if (!ok)
            return false;

        frame.setValue(val);
    }
    else if (t.compare("temp")==0)
    {
        bool ok;
        float val = v.toFloat(&ok);
        if (!ok)
            return false;

        frame.setTemp(val);
    }
    else if (t.compare("dimm")==0)
    {
        if (v.compare("startup"))
        {
            frame.setDimm(ObjectConnectionSupport::StartUp);
        }
        else if (v.compare("stopup")==0)
        {
            frame.setDimm(ObjectConnectionSupport::StopUp);
        }
        else if (v.compare("startdown")==0)
        {
            frame.setDimm(ObjectConnectionSupport::StartDown);
        }
        else if (v.compare("stopdown")==0)
        {
            frame.setDimm(ObjectConnectionSupport::StopDown);
        }
        else
            return false;

        return true;
    }
    else if (t.compare("time"))
    {
        QTime time = QTime::fromString(v, "hh:mm:ss");
        if (!time.isValid())
            return false;

        frame.setTime(time);
    }
    else if (t.compare("date"))
    {
        QDate date = QDate::fromString(v, "yyyy-MM-dd");
        if (!date.isValid())
            return false;

        frame.setDate(date);
    }
    else
        return false;

    return true;
}

bool SngDefinitions::valueFromString(const QString &type, const QString &value, quint8 data[ConnFrameDataFieldsLength])
{
    ConnectionFrame frame;
    bool ok = setValueFromString(type, value, frame);
    if (ok)
        memcpy(data, frame.dataFields(), ConnFrameDataFieldsLength);
    else
        memset(data, 0, ConnFrameDataFieldsLength);
    return ok;
}
