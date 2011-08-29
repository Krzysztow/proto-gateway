#include "helpercoder.h"

quint8 HelperCoder::sint32fromVarLengthRaw(quint8 *ptr, qint32 *result, quint8 varLength)
{
    Q_CHECK_PTR(ptr);
    Q_CHECK_PTR(result);

    int i;
    *result = 0;
    if (*(qint8*)ptr < 0) {//if the value is negative - we have to fill most significant places with ones
//        qint32 filler = ((quint8)-1) << 8*(sizeof(qint32)-1);
//        for (i = sizeof(qint32)-1; i>varLength-1; --i) {
//            *result |= filler;
//            filler >>= 8;
//        }
        *result = -1;
    }

    quint8 *dataPtr = ptr;
    for (int i = varLength-1; i >= 0; --i) {
        *result |= ((*dataPtr) << i*8);
        dataPtr++;
    }

    return dataPtr - ptr;
}

quint8 HelperCoder::uint32ToVarLengthRaw(quint8 *dstPtr, quint32 value)
{
    Q_CHECK_PTR(dstPtr);

    quint8 *actualPtr = dstPtr;
    quint32 mask = 0xff000000;

    if (0 == value) {//nothing has been encoded yet! - all 0s
        *actualPtr = 0x00;
        return 1;
    }

    for (int i = 0; i<sizeof(quint32); ++i) {
        if ((actualPtr != dstPtr) || (mask & value)) {
            *actualPtr = (quint8)((mask & value) >> (8*(sizeof(quint32)-1-i)));
            ++actualPtr;
        }
        mask >>= 8;
    }

    return actualPtr - dstPtr;
}

quint8 HelperCoder::sint32ToVarLengthRaw(quint8 *dstPtr, qint32 value)
{
    Q_CHECK_PTR(dstPtr);

    quint8 *actualPtr = dstPtr;
    quint32 mask = 0xff000000;
    qint32 help = (value < 0) ? -value : value;

    if (-1 == value || 0 == value) {
        *actualPtr = value;
        return 1;
    }

    for (int i = 0; i<sizeof(qint32); ++i) {
        if ((actualPtr != dstPtr) || (mask & help)) {
            *actualPtr = (quint8)((mask & value) >> (8*(sizeof(quint32)-1-i)));
            ++actualPtr;
        }
        mask >>= 8;
    }

    return actualPtr - dstPtr;
}

quint8 HelperCoder::uint32fromVarLengthRaw(quint8 *ptr, quint32 *result, quint8 varLength)
{
    Q_CHECK_PTR(ptr);
    Q_CHECK_PTR(result);

    *result = 0;//initialize to 0
    quint8 *dataPtr = ptr;
    for (int i = varLength-1; i >= 0; --i) {
        *result |= ((*dataPtr) << i*8);
        dataPtr++;
    }

    return dataPtr - ptr;
}

quint8 HelperCoder::doubleFromRaw(quint8 *ptr, double *result)
{
    Q_ASSERT(8 == sizeof(double));
    Q_CHECK_PTR(ptr);
    Q_CHECK_PTR(result);
//    quint32 temp;
//    quint8 length;
//    length = uint32FromRaw(ptr, &temp);
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    memcpy(result, ptr, sizeof(double));
#else // Q_LITTLE_ENDIAN
    ((quint8*)result)[0] = ptr[7];
    ((quint8*)result)[1] = ptr[6];
    ((quint8*)result)[2] = ptr[5];
    ((quint8*)result)[3] = ptr[4];
    ((quint8*)result)[4] = ptr[3];
    ((quint8*)result)[5] = ptr[2];
    ((quint8*)result)[6] = ptr[1];
    ((quint8*)result)[7] = ptr[0];
#endif

    return 8;
}

quint8 HelperCoder::doubleToRaw(double &value, quint8 *destPtr)
{
    Q_ASSERT(8 == sizeof(double));
    Q_CHECK_PTR(destPtr);
    quint8 *srcPtr = (quint8*)&value;
//    quint32 temp;
//    quint8 length;
//    length = uint32FromRaw(ptr, &temp);
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    memcpy(destPtr, srcPtr, sizeof(double));
#else // Q_LITTLE_ENDIAN
    destPtr[7] = ((quint8*)srcPtr)[0];
    destPtr[6] = ((quint8*)srcPtr)[1];
    destPtr[5] = ((quint8*)srcPtr)[2];
    destPtr[4] = ((quint8*)srcPtr)[3];
    destPtr[3] = ((quint8*)srcPtr)[4];
    destPtr[2] = ((quint8*)srcPtr)[5];
    destPtr[1] = ((quint8*)srcPtr)[6];
    destPtr[0] = ((quint8*)srcPtr)[7];
#endif

    return 8;
}
