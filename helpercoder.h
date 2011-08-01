#ifndef HELPERCODER_H
#define HELPERCODER_H

#include <QtCore>

namespace HelperCoder
{
    /**
      Funcitons decode unsigned integers. They convert data represented in netowrk byte order (big endian)
      to integers in host enian.
      Returns number of bytes used.
      */
    static inline quint8 uint16FromRaw(quint8 *ptr, quint16 *result);
    static inline quint8 sint16FromRaw(quint8 *ptr, qint16 *result);
    static inline quint8 uint32FromRaw(quint8 *ptr, quint32 *result);
    static inline quint8 sint32FromRaw(quint8 *ptr, qint32 *result);

    static inline quint8 uin16ToRaw(quint16 source, quint8 *destPtr);
    static inline quint8 sin16ToRaw(qint16 source, quint8 *destPtr);
    static inline quint8 uint32ToRaw(quint32 source, quint8 *destPtr);
    static inline quint8 uint32ToRaw(qint32 source, quint8 *destPtr);
};

inline quint8 HelperCoder::uint16FromRaw(quint8 *ptr, quint16 *result)
{
    Q_CHECK_PTR(ptr);
    (*result) = qFromBigEndian(*(quint16*)ptr);
    return (sizeof(quint16));
}

inline quint8 HelperCoder::sint16FromRaw(quint8 *ptr, qint16 *result)
{
    Q_CHECK_PTR(ptr);
    (*result) = qFromBigEndian(*(qint16*)ptr);
    return sizeof(qint16);
}

inline quint8 HelperCoder::uint32FromRaw(quint8 *ptr, quint32 *result)
{
    Q_CHECK_PTR(ptr);
    (*result) = qFromBigEndian(*(quint32*)ptr);
    return (sizeof(quint32));
}

inline quint8 HelperCoder::sint32FromRaw(quint8 *ptr, qint32 *result)
{
    Q_CHECK_PTR(ptr);
    (*result) = qFromBigEndian(*(qint32*)ptr);
    return sizeof(qint32);
}

inline quint8 HelperCoder::uin16ToRaw(quint16 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    *(quint16*)(destPtr) = qToBigEndian(source);
    return sizeof(quint16);
}

inline quint8 HelperCoder::sin16ToRaw(qint16 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    *(qint16*)(destPtr) = qToBigEndian(source);
    return sizeof(qint16);
}

inline quint8 HelperCoder::uint32ToRaw(quint32 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    *(quint32*)(destPtr) = qToBigEndian(source);
    return sizeof(quint32);
}
inline quint8 HelperCoder::uint32ToRaw(qint32 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    *(qint32*)(destPtr) = qToBigEndian(source);
    return sizeof(qint32);
}

#endif // HELPERCODER_H
