#ifndef BACNETREADPROPERTY_H
#define BACNETREADPROPERTY_H

#include <QtCore>
#include "bacnetcommon.h"

class BacnetReadProperty
{
public:
    qint16 fromRaw(quint8 *dataPtr, quint16 lengthLeft);

private:

};

#endif // BACNETREADPROPERTY_H
