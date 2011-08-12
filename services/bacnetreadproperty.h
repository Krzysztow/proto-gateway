#ifndef BACNETREADPROPERTY_H
#define BACNETREADPROPERTY_H

#include <QtCore>

class BacnetReadProperty
{
public:
    qint16 fromRaw(quint8 *dataPtr, quint16 lengthLeft);
};

#endif // BACNETREADPROPERTY_H
