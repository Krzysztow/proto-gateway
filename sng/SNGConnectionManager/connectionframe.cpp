#include <math.h>

#include "objectconnectionsupport.h"
#include "connectionframe.h"

ConnectionFrame::ConnectionFrame()
{
    _data.start0      = 0xFF;
    _data.start1      = 0xFF;
    _data.paddr0      = 0x00;
    _data.paddr1      = 0x00;
    _data.paddr2      = 0x00;
    _data.gaddr0      = 0x00;
    _data.gaddr1      = 0x00;
    _data.gaddr2      = 0x00;
    _data.data_type   = 0x00;
    _data.data0       = 0x00;
    _data.data1       = 0x00;
    _data.data2       = 0x00;
    _data.data3       = 0x00;
    _data.finish      = 0xFE;
}

/*!
  @attention All changes in addressing should to by implemented in ControllManager::Index too.
  */
GroupAddress ConnectionFrame::address() const
{
    return GroupAddress(_data.gaddr0, _data.gaddr1, _data.gaddr2);
}

void ConnectionFrame::setPhyAddr(const PhysicalAddress &address)
{
    _data.paddr0 = address.main;
    _data.paddr1 = address.middle;
    _data.paddr2 = address.sub;
}


GroupAddress ConnectionFrame::phyAddress() const
{
    return GroupAddress(_data.paddr0, _data.paddr1, _data.paddr2);
}


/**
  @attention All changes in addressing should to by implemented in ControllManager::Index too.
  */
void ConnectionFrame::setGrAddress(const GroupAddress &address)
{
    _data.gaddr0 = address.main;
    _data.gaddr1 = address.middle;
    _data.gaddr2 = address.sub;
}

char* ConnectionFrame::data()
{
    return (char *) &_data;
}

const char* ConnectionFrame::data() const
{
    return (const char *) &_data;
}

char *ConnectionFrame::dataFields()
{
    return (char*)&_data.data0;
}

const char *ConnectionFrame::dataFields() const
{
    return (const char*)&_data.data0;
}

void ConnectionFrame::setDataType(DataType type)
{
    _data.data_type = (quint8)type;
}

int ConnectionFrame::sizeOfData() const
{
    return sizeof(_data);
}

ConnectionFrame::DataType ConnectionFrame::dataType() const
{
    return (ConnectionFrame::DataType) _data.data_type;
}

bool ConnectionFrame::isValid() const
{
    return _data.start0 == 0xFF && _data.start1 == 0xFF && _data.finish == 0xFE; // ...
}












void ConnectionFrame::setOnOff(bool value)
{
    _data.data_type = 0x01;
    _data.data0 = (value?0x01:0x00);
}

void ConnectionFrame::setDimm(ObjectConnectionSupport::DimmCommand value)
{
    _data.data_type = 0x02;
    switch (value)
    {
        case ObjectConnectionSupport::StartUp:
        _data.data0 = StartUp;
        break;
        case ObjectConnectionSupport::StopUp:
        _data.data0 = StopUp;
        break;

        case ObjectConnectionSupport::StartDown:
        _data.data0 = StartDown;
        break;
        case ObjectConnectionSupport::StopDown:
        _data.data0 = StopDown;
        break;
        default:
        _data.data0 = StopDown;
        break;
    }
}

void ConnectionFrame::setTime(const QTime& value)
{
    _data.data_type = 0x03;
    _data.data0 = (quint8) value.hour();
    _data.data1 = (quint8) value.minute();
    _data.data2 = (quint8) value.second();
}

void ConnectionFrame::setDate(const QDate& value)
{
    _data.data_type = 0x04;
    _data.data0 = (quint8) value.day();
    _data.data1 = (quint8) value.month();
    _data.data2 = (quint8) value.year();
}

void ConnectionFrame::setTemp(float value)
{
    _data.data_type = 0x05;
    if (value >= 0)
        _data.data2 = 0x00;
    else
        _data.data2 = 0x01;
    value = fabs(value);
    _data.data0 = (quint8) floorf(value);
    _data.data1 = (quint8) roundf((value - floorf(value)) * 10);
}

void ConnectionFrame::setValue(int value)
{
    _data.data_type = 0x06;
    _data.data0 = (quint8) value;
}


bool ConnectionFrame::onOff() const
{
    return _data.data0 == 0x01;
}

ObjectConnectionSupport::DimmCommand ConnectionFrame::dimm() const
{
    switch (_data.data0)
    {
        case StartUp:
        return ObjectConnectionSupport::StartUp;
        break;
        case StopUp:
        return ObjectConnectionSupport::StopUp;
        break;

        case StartDown:
        return ObjectConnectionSupport::StartDown;
        break;
        case StopDown:
        return ObjectConnectionSupport::StopDown;
        break;
        default:
        return (ObjectConnectionSupport::DimmCommand) 31;
        break;
    }
}

QTime ConnectionFrame::time() const
{
    return QTime(_data.data0, _data.data1, _data.data2, 0);
}

QDate ConnectionFrame::date() const
{
    return QDate(_data.data2, _data.data1, _data.data0);
}

float ConnectionFrame::temp() const
{
    float value = (float) _data.data0;
    if (_data.data1 != 0)
           value += pow(10, -floor(log10(_data.data1))-1) * _data.data1;
           value *= (_data.data2 == 0x00 ? 1 : -1);
    return value;
}

int ConnectionFrame::value() const
{
    return (int) _data.data0;
}
