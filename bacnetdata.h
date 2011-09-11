#ifndef BACNETDATA_H
#define BACNETDATA_H

#include <QtCore>
#include "bacnetcommon.h"

class BacnetTagParser;
namespace Bacnet
{
    class BacnetDataInterface
    {
    public:
        BacnetDataInterface();
        virtual ~BacnetDataInterface();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength) = 0;
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber) = 0;
        /*Gets BacnetTagParser and tries to parse data so that all its fields are filled. Then returns
        number of bytes used. It'd doesn't remember the tag number it was passed, however may make some
        checks - if application data, make sure we are the one called correctly.*/
        virtual qint32 fromRaw(BacnetTagParser &parser) = 0;
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum) = 0;

        virtual bool setInternal(QVariant &value) = 0;
        virtual QVariant toInternal() = 0;

        virtual DataType::DataType typeId() = 0;
    };
}

#endif // BACNETDATA_H
