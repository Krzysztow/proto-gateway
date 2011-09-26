#ifndef SEQUENCEOF_H
#define SEQUENCEOF_H

#include "bacnetdata.h"
#include "bacnetcoder.h"
#include "bacnettagparser.h"

namespace Bacnet {
    template <class T>
            class SequenceOf
    {
    public:
        ~SequenceOf();

        qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
        qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);

        qint32 fromRawSpecific(BacnetTagParser &parser, BacnetObjectType::ObjectType objectType);
        qint32 fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectType::ObjectType objectType);

        bool setInternal(QVariant &value);
        QVariant toInternal();

        DataType::DataType typeId();

    private:
        QList<T*> _sequence;
    };


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////Template definition///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SequenceOf<T>::~SequenceOf()
    {
        qDeleteAll(_sequence);
    }

    template <class T>
    qint32 SequenceOf<T>::toRaw(quint8 *ptrStart, quint16 buffLength)
    {
        quint8 *actualPtr(ptrStart);
        qint16 ret(0);
        for (int i=0; i<_sequence.count(); ++i) {
            ret = _sequence[i]->toRaw(actualPtr, buffLength);
            if (ret < 0)
                return ret;
            actualPtr += ret;
            buffLength -= ret;
        }
        return (actualPtr - ptrStart);
    }

    template <class T>
    qint32 SequenceOf<T>::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
    {
        qint32 ret(0);
        quint8 *actualPtr(ptrStart);
        ret = BacnetCoder::openingTagToRaw(actualPtr, buffLength, tagNumber);
        if (ret <  0 ) return ret;
        actualPtr += ret;
        buffLength -= ret;
        ret = toRaw(ptrStart, buffLength);
        if (ret < 0) return ret;
        actualPtr += ret;
        buffLength -= ret;
        ret = BacnetCoder::closingTagToRaw(actualPtr, buffLength, tagNumber);
        if (ret < 0) return ret;
        actualPtr += ret;
        return (actualPtr - ptrStart);
    }

    template <class T>
    qint32 SequenceOf<T>::fromRawSpecific(BacnetTagParser &parser, BacnetObjectType::ObjectType objectType)
    {
        Q_ASSERT_X(false, "", "Not implemented yet!");
        return -1;
    }

    template <class T>
    qint32 SequenceOf<T>::fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectType::ObjectType objectType)
    {
        bool okOrContext;
        quint32 total(0);
        qint32 ret;

        ret = parser.parseNext();
        if (ret < 0 || !parser.isOpeningTag(tagNum))
            return -1;
        total += ret;

        T *seqElem = 0;
        ret = parser.nextTagNumber(&okOrContext);
        while ((tagNum != ret) || (!okOrContext)) {
            seqElem = new T();
            _sequence.append(seqElem);//append it first, so that in case of error not loose it.
#warning "Take care of this - parameter objectType - is it really needed?"
            ret = seqElem->fromRawSpecific(parser, objectType);
            if (ret < 0)
                return ret;
            total += ret;
            ret = parser.nextTagNumber(&okOrContext);
        }

        ret = parser.parseNext();
        if (ret < 0 || !parser.isClosingTag(tagNum))
            return -1;
        total += ret;
        return total;
    }

    template <class T>
    bool SequenceOf<T>::setInternal(QVariant &value)
    {
        return false;
    }

    template <class T>
    QVariant SequenceOf<T>::toInternal()
    {
        return QVariant();
    }

    template <class T>
    DataType::DataType SequenceOf<T>::typeId()
    {
        return DataType::BACnetSequence;
    }

}

#endif // SEQUENCEOF_H
