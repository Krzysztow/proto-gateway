#ifndef CONNECTIONFRAME_H
#define CONNECTIONFRAME_H

#include <QTime>
#include <QDate>

#include "objectconnectionsupport.h"
#include "groupaddress.h"

class ConnectionFrame
{
/**
  @attention All changes in addressing should to by implemented in ControllManager::Index too.
  */
private:
    struct {
        quint8  start0;
        quint8  start1;
        quint8  paddr0;
        quint8  paddr1;
        quint8  paddr2;
        quint8  gaddr0;
        quint8  gaddr1;
        quint8  gaddr2;
        quint8  data_type;
        quint8  data0;
        quint8  data1;
        quint8  data2;
        quint8  data3;
        quint8  finish;
    } _data;

public:
    //    enum DataType
    //    {
    //        OnOff   = ConnectionFrame::OnOff,
    //        Dimm    = ConnectionFrame::Dimm   ,
    //        Time    = ConnectionFrame::Time   ,
    //        Date    = ConnectionFrame::Date   ,
    //        Temp    = ConnectionFrame::Temp   ,
    //        Value_Unsigned   = ConnectionFrame::Value  ,
    //        Float4B = ConnectionFrame::Float4B,
    //        Value2B_Unsigned = ConnectionFrame::Value2B,
    //        Value4B_Unsigned = ConnectionFrame::Value4B,
    //        Value_Signed,
    //        Value2B_Signed,
    //        Value4B_Signed
    //    };


    enum DataType {
        OnOff = 0x01,
        Dimm  = 0x02,
        Time  = 0x03,
        Date  = 0x04,
        Temp  = 0x05,
        Value = 0x06,       //value unsigned
        Float4B = 0x09,     //float 4B
        Value2B = 0x0a,     //value unsigned 2B
        Value4B = 0x0b,     //value unsigned 4B
        Value_Signed = 0x0c,//value signed 1B
        Value2B_Signed = 0x0d,//value signed 2B
        Value4B_Signed = 0x0e //value signed 4B
    };


public:
    enum DimmValues {StartUp   = 9,
                     StopUp    = 8,
                     StartDown = 1,
                     StopDown  = 0};

    enum ModeValues {
      ModeComfort   = 0x01,
      ModeStandby   = 0x02,
      ModeNight     = 0x03,
      ModeParty     = 0x0A
    };

public:
    ConnectionFrame();

public:
    const char* data() const;
    char* data();
    int sizeOfData() const;
    DataType dataType() const;
    void setDataType(DataType type);
    bool isValid() const;

    char *dataFields();
    const char *dataFields() const;

    /*!
    @attention All changes in addressing should to by implemented in ControllManager::Index too.
    This function sets the group address fields in the frame instance.
    \param address group address set as string, has form of "1/2/3";
    \return true if address conversion successful, otherwise false.
    */
    void setGrAddress(const GroupAddress& address);
    /*!
    @attention All changes in addressing should to by implemented in ControllManager::Index too.
    Returns numerical representation (structure \sa GroupAddr) of the frame address field.
    */
    GroupAddress address() const;

    /**
      \note This function is meant to be used by ConnectionManager when sending data. Everything what you set is overwritten by ConnectionManager.
      Functions sets bytes of the ConnectionFrame instance, corresponding to physical address.
      \param address string representing the physical address values, should be of form "1.2.3";
      \return returns true if conversion is accomplished correctly, otherwise false.
      */
    void setPhyAddr(const PhysicalAddress &address);
    /**
      Function returns string representing physical address fields of the connection frame.
      */
    PhysicalAddress phyAddress() const;

public:
    void setOnOff(bool value);
    void setDimm(ObjectConnectionSupport::DimmCommand value);
    void setTime(const QTime& value);
    void setDate(const QDate& value);
    void setTemp(float value);
    void setValue(int value);

public:
    bool onOff() const;
    ObjectConnectionSupport::DimmCommand dimm() const;
    QTime time() const;
    QDate date() const;
    float temp() const;
    int value() const;
};


#endif // CONNECTIONFRAME_H
