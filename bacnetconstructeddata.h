#ifndef BACNETCONSTRUCTEDDATA_H
#define BACNETCONSTRUCTEDDATA_H

#include <QtCore>
#include "bacnetdata.h"
#include "bacnetcoder.h"
#include "bacnetcommon.h"
#include "bacnetprimitivedata.h"

namespace Bacnet
{
    class DeviceStatus:
            public Enumerated
    {
    public://owerwritten from Enumerated
        virtual DataType::DataType typeId();

    public:
        enum BACnetDeviceStatus {
            Operational = 0,
            OperationalReadOnly = 1,
            DownloadRequired = 2,
            DownloadInProgress = 3,
            NonOperational = 4,
            BackupInProgress = 5
                           };
    void setDeviceStatus(BACnetDeviceStatus status);

    private:
        static const int REQ_BITS_NUM = 4;
    };

    class Segmentation:
            public Enumerated
    {
    public:
        virtual DataType::DataType typeId();

    public:
        enum BACnetSegmentation {
            SegmendetBoth       = 0,
            SegmentedTransmit   = 1,
            SegmentedReceive    = 2,
            NoSegmentation      = 3
        };

        Segmentation(BACnetSegmentation segOption = NoSegmentation);
        void setSegmentation(BACnetSegmentation segOption);
        BACnetSegmentation segmentation();
    };








    class ObjectTypesSupported:
            public BitString
    {
    public://owerwritten from BitString
        virtual bool setInternal(QVariant &value);
        virtual DataType::DataType typeId();

    public:
        void clearObjectsSupported();
        void addObjectsSupported(QList<BacnetObjectType::ObjectType> objSupported);

    private:
        static const int REQ_BITS_NUM = BacnetObjectType::MaximumEnumType + 1;
    };

    class Unsigned16:
            public UnsignedInteger
    {
    public://overwritten from UnsignedInteger
        virtual bool setInternal(QVariant &value);
        virtual DataType::DataType typeId();

    private:
        static const unsigned int MAX_VALUE = 0xffffffff;
    };

    class ServicesSupported:
            public BitString
    {
    public:
        ServicesSupported();

    public:
        void clearServices();
        void setServices(QList<quint8> &_servicesToSet);
    };

    class DateTime:
            public BacnetDataInterface
    {
    public:
        DateTime();
        virtual ~DateTime();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
        /*Gets BacnetTagParser and tries to parse data so that all its fields are filled. Then returns
        number of bytes used. It'd doesn't remember the tag number it was passed, however may make some
        checks - if application data, make sure we are the one called correctly.*/
        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        Date _date;
        Time _time;
    };



    class TimeStamp:
            public BacnetDataInterface
    {
        TimeStamp();
        virtual ~TimeStamp();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
        /*Gets BacnetTagParser and tries to parse data so that all its fields are filled. Then returns
        number of bytes used. It'd doesn't remember the tag number it was passed, however may make some
        checks - if application data, make sure we are the one called correctly.*/
        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        BacnetDataInterface *_choiceValue;
    };

}

#endif // BACNETCONSTRUCTEDDATA_H
