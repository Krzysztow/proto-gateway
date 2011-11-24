#include "invokeidgenerator.h"

InvokeIdGenerator::InvokeIdGenerator(int maxIdInclusive):
    _maxIdsNumber(maxIdInclusive),
    _numberOfOctetsTaken(_maxIdsNumber/8 + ( (_maxIdsNumber%8 == 0) ? 0 : 1)),
    _idsBits(new quint8[_numberOfOctetsTaken])
{
    memset(_idsBits, 0, _numberOfOctetsTaken);
}

InvokeIdGenerator::~InvokeIdGenerator()
{
    delete [] _idsBits;
}

int InvokeIdGenerator::generateId()
{
    quint8 *bytePtr = _idsBits;
    quint8 mask = 0x01;
    quint8 bitNumber;

    for (int i = 0; i < _numberOfOctetsTaken; ++i) {
        if (*bytePtr != 0xff) { //there is a free entry
            for (bitNumber = 0; bitNumber < 8; ++bitNumber) {
                if ( (mask & *bytePtr) == 0) { //bit pointing by mask is free
                    *bytePtr |= mask;//reserve field
                    return (8*i + bitNumber);
                }
                mask <<= 1;
            }
        }
        ++bytePtr;
    }
    return -1;
}

void InvokeIdGenerator::returnId(quint8 id)
{
    quint8 byteNumber = id/8;
    quint8 mask = (0x01 << id%8);

    Q_ASSERT( (_idsBits[byteNumber] & mask) != 0 );//assert it is used.
    _idsBits[byteNumber] &= (~mask);//free bit
}
