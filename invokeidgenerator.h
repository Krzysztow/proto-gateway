#ifndef INVOKEIDGENERATOR_H
#define INVOKEIDGENERATOR_H

#include <QtCore>

class InvokeIdGenerator
{
public:
    InvokeIdGenerator(int maxIdInclusive = 255);
    ~InvokeIdGenerator();

    int generateId();
    void returnId(quint8 id);

public:

private:
    const int _maxIdsNumber;
    int _numberOfOctetsTaken;
    quint8 *_idsBits;
};

#endif // INVOKEIDGENERATOR_H
