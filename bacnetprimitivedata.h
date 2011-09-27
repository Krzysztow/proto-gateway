#ifndef BACNETPRIMITIVEDATA_H
#define BACNETPRIMITIVEDATA_H

#include <QtCore>
#include "bacnetdata.h"
#include "bacnetcoder.h"
#include "bacnetcommon.h"

namespace Bacnet
{
    class Null: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    };

    class Boolean: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    private:
        bool _value;
    };

    class UnsignedInteger: public BacnetDataInterface
    {
    public:
        UnsignedInteger(quint32 value = 0);

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    public:
        void setValue(quint32 value);
        quint32 value();

    protected:
        quint32 _value;
    };

    class SignedInteger: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        qint32 _value;
    };

    class Real: public BacnetDataInterface
    {
    public:
        Real(float value = 0);

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        float _value;
    };

    class Double: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        double _value;
    };

    class OctetString: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    private:
        QByteArray _value;
    };

    class CharacterString: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        qint32 toRaw_helper(quint8 *ptrStart, quint16 buffLength, bool isContext, quint8 tagNumber);

    public:
        QString _value;
        BacnetCoder::CharacterSet _charSet;
    };

    class BitString: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    public:
        QBitArray &value();

    private:
        void toRaw_helper(quint8 *dataStart);

    protected:
        QBitArray _value;
    };

    class Enumerated: public BacnetDataInterface
    {
    public:
        Enumerated(quint32 enumValue = 0);
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    protected:
        quint32 _value;
    };

    class Date: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        void toRaw_helper(quint8 *dataStart);

    private:
        QDate _value;
    };

    class Time: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();
    private:
        void toRaw_helper(quint8 *dataStart);

    private:
        QTime _value;
    };

    class ObjectIdentifier: public BacnetDataInterface
    {
    public:
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    public:
        ObjectIdentifier();
        ObjectIdentifier(Bacnet::ObjectIdStruct &objId);

    private:
        Bacnet::ObjectIdStruct _value;
    };

    /**This is a special proxy class.
      */
    class BacnetDataBaseDeletable: public BacnetDataInterface
    {
    public:
        BacnetDataBaseDeletable(BacnetDataInterface *shared);
        virtual ~BacnetDataBaseDeletable();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    private:
        BacnetDataInterface *_value;
    };

    class BacnetList: public BacnetDataInterface
    {
    public:
        BacnetList(DataType::DataType type = DataType::InvalidType);
        virtual ~BacnetList();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        virtual qint32 fromRaw(BacnetTagParser &parser);
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

        virtual bool setInternal(QVariant &value);
        virtual QVariant toInternal();

        virtual DataType::DataType typeId();

    public:
        void setStoredType(DataType::DataType type);
        DataType::DataType storedType();

    public:
        bool addElement(BacnetDataInterface *value);

    protected:
        QList<BacnetDataInterface*> _value;
        DataType::DataType _storedType;
    };

    class BacnetArray:
            public BacnetList
    {
    public:
        BacnetArray(DataType::DataType type = DataType::InvalidType);

        virtual DataType::DataType typeId();

    public:
        BacnetDataInterface *createElementAt(quint8 position);
    };
}

#endif
