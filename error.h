#ifndef ERROR_H
#define ERROR_H

#include "bacnetcommon.h"
namespace Bacnet {

    class Error {
    public:
        Error();
        Error(BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode);

        bool hasError();
        void setError(BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode);
    public:
        BacnetErrorNS::ErrorClass errorClass;
        BacnetErrorNS::ErrorCode errorCode;
    };

}

#endif // ERROR_H
