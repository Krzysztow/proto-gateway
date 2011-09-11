#ifndef BACNETDATAABSTRACT_H
#define BACNETDATAABSTRACT_H

#include "bacnetdata.h"
#include "bacnetcommon.h"

namespace Bacnet {
    class DataAbstract:
            public BacnetDataInterface
    {
    public:
        DataAbstract();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNumber);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    public:
        QByteArray &data();

    private:
        bool _isContextSpecific;
        quint8 _tagNumber;
        QByteArray _value;
    };
}

#endif // BACNETDATAABSTRACT_H
