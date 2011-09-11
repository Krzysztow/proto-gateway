#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include "bacnetcommon.h"

class BacnetService;
namespace ServiceFactory
{
    BacnetService *createService(quint8 *servicePtr, quint16 length,
                         quint8 serviceCode, qint32 *retCode);
};

#endif // SERVICEFACTORY_H
