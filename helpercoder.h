#ifndef HELPERCODER_H
#define HELPERCODER_H

#include <QtCore>

#ifndef QT_NO_DEBUG
QDebug operator<<(QDebug dbg, const QBitArray& z);
#endif


namespace HelperCoder
{
//#ifndef QT_NO_DEBUG
    void printArray(const quint8 *ptr, int size, const char *pretext);
    void printBin(int value, int lsbBitsNum, const char *prestring, const char *poststring);
//#else
//#define printArray(a,b,c) ;
//#define printBin(a,b,c,d) ;
//#endif

    /**
      Funcitons decode unsigned integers. They convert data represented in netowrk byte order (big endian)
      to integers in host enian.
      Returns number of bytes used.
      */
    static inline quint8 uint16FromRaw(const quint8 *ptr, quint16 *result);
    static inline quint8 sint16FromRaw(const quint8 *ptr, qint16 *result);
    static inline quint8 uint32FromRaw(const quint8 *ptr, quint32 *result);
    static inline quint8 sint32FromRaw(const quint8 *ptr, qint32 *result);

    quint8 uint32fromVarLengthRaw(const quint8 *ptr, quint32 *result, quint8 varLength);
    quint8 sint32fromVarLengthRaw(const quint8 *ptr, qint32 *result, quint8 varLength);
    quint8 doubleFromRaw(const quint8 *ptr, double *result);
    quint8 doubleToRaw(double &value, quint8 *destPtr);
    static inline quint8 floatFromRaw(const quint8 *ptr, float *result);
    static inline quint8 floatToRaw(float value, quint8 *destPtr);

    static inline quint8 uin16ToRaw(quint16 source, quint8 *destPtr);
    static inline quint8 sin16ToRaw(qint16 source, quint8 *destPtr);
    static inline quint8 uint32ToRaw(quint32 source, quint8 *destPtr);
    static inline quint8 sint32ToRaw(qint32 source, quint8 *destPtr);

    quint8 uint32ToVarLengthRaw(quint8 *dstPtr, quint32 value);
    quint8 sint32ToVarLengthRaw(quint8 *dstPtr, qint32 value);
}


struct sUint16 { quint8 first; quint8 second;};
union uUint16 {sUint16 s; quint16 value;};

inline quint8 HelperCoder::uint16FromRaw(const quint8 *ptr, quint16 *result)
{
    Q_CHECK_PTR(ptr);
    uUint16 raw = {{*ptr, *(ptr + 1)}};
    (*result) = qFromBigEndian(raw.value);
    return (sizeof(quint16));
}


struct sSint16 { quint8 first; quint8 second;};
union uSint16 {sSint16 s; qint16 value;};

inline quint8 HelperCoder::sint16FromRaw(const quint8 *ptr, qint16 *result)
{
    Q_CHECK_PTR(ptr);
    uSint16 raw = {{*ptr, *(ptr+1)}};
    (*result) = qFromBigEndian(raw.value);
    return sizeof(qint16);
}


struct sSint32 { quint8 first; quint8 second; quint8 third; quint8 fourth;};
union uSint32 {sSint32 s; qint32 value;};

inline quint8 HelperCoder::sint32FromRaw(const quint8 *ptr, qint32 *result)
{
    Q_CHECK_PTR(ptr);
    uSint32 raw = {{*ptr, *(ptr+1), *(ptr+2), *(ptr+3)}};
    (*result) = qFromBigEndian(raw.value);
    return sizeof(qint32);
}

struct sUint32 { quint8 first; quint8 second; quint8 third; quint8 fourth;};
union uUint32 {sUint32 s; quint32 value;};

inline quint8 HelperCoder::uint32FromRaw(const quint8 *ptr, quint32 *result)
{
    Q_CHECK_PTR(ptr);
    uUint32 raw = {{*ptr, *(ptr+1), *(ptr+2), *(ptr+3)}};
    (*result) = qFromBigEndian(raw.value);
    return (sizeof(quint32));
}

inline quint8 HelperCoder::floatFromRaw(const quint8 *ptr, float *result)
{
    Q_ASSERT(4 == sizeof(float));//we confine ourselves only to 4-byte floats. 8-byte ones are double!
    return uint32FromRaw(ptr, (quint32*)result);
}

union uFloat {float f; quint32 u;};

inline quint8 HelperCoder::floatToRaw(float value, quint8 *destPtr)
{
    Q_ASSERT(4 == sizeof(float));//we confine ourselves only to 4-byte floats. 8-byte ones are double!
    uFloat orig = {value};
    return uint32ToRaw(orig.u, destPtr);
}


//struct sUint16 { quint8 first; quint8 second;};
//union uUint16 {sUint16 s; quint16 value;};

inline quint8 HelperCoder::uin16ToRaw(quint16 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    source = qToBigEndian(source);
    memcpy(destPtr, &source, sizeof(quint16));
    return sizeof(quint16);
}


//struct sSint16 { quint8 first; quint8 second;};
//union uSint16 {sSint16 s; qint16 value;};

inline quint8 HelperCoder::sin16ToRaw(qint16 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    source = qToBigEndian(source);
    memcpy(destPtr, &source, sizeof(qint16));
    return sizeof(qint16);
}


//struct sUint32 { quint8 first; quint8 second; quint8 third; quint8 fourth;};
//union uUint32 {sUint32 s; quint32 value;};

inline quint8 HelperCoder::uint32ToRaw(quint32 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    source = qToBigEndian(source);
    memcpy(destPtr, &source, sizeof(quint32));
    return sizeof(quint32);
}

//struct sSint32 { quint8 first; quint8 second; quint8 third; quint8 fourth;};
//union uSint32 {sSint32 s; qint32 value;};

inline quint8 HelperCoder::sint32ToRaw(qint32 source, quint8 *destPtr)
{
    Q_CHECK_PTR(destPtr);
    source = qToBigEndian(source);
    memcpy(destPtr, &source, sizeof(qint32));
    return sizeof(qint32);
}



#endif // HELPERCODER_H

