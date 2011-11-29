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

    qint32 fromRawSpecific(BacnetTagParser &parser, BacnetObjectTypeNS::ObjectType objectType);
    qint32 fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectTypeNS::ObjectType objectType);

    bool setInternal(QVariant &value);
    QVariant toInternal();

    DataType::DataType typeId();

public:
    QList<QSharedPointer<T> > &value() {return _sequence;}

public:
    void append(QSharedPointer<T> &t);

public:
    QList<QSharedPointer<T> > _sequence;
};


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////Template definition///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template <class T>
SequenceOf<T>::~SequenceOf()
{
//    qDeleteAll(_sequence);//no needed anymore - QSharedPointer will delete allocated resources if needed.
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
    ret = toRaw(actualPtr, buffLength);
    if (ret < 0) return ret;
    actualPtr += ret;
    buffLength -= ret;
    ret = BacnetCoder::closingTagToRaw(actualPtr, buffLength, tagNumber);
    if (ret < 0) return ret;
    actualPtr += ret;
    return (actualPtr - ptrStart);
}

template <class T>
qint32 SequenceOf<T>::fromRawSpecific(BacnetTagParser &parser, BacnetObjectTypeNS::ObjectType objectType)
{
    Q_ASSERT_X(false, "", "Not implemented yet!");
    return -1;
}

template <class T>
qint32 SequenceOf<T>::fromRawSpecific(BacnetTagParser &parser, quint8 tagNum, BacnetObjectTypeNS::ObjectType objectType)
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
        _sequence.append(QSharedPointer<T>(seqElem));//append it first, so that in case of error we keep track of it and are able to delete later on.
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

template <class T>
void SequenceOf<T>::append(QSharedPointer<T> &t)
{
    Q_CHECK_PTR(t);
    _sequence.append(t);
}

}

#endif // SEQUENCEOF_H
