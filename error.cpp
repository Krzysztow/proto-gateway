#include "error.h"

using namespace Bacnet;

Error::Error():
        errorClass(BacnetErrorNS::ClassNoError),
        errorCode(BacnetErrorNS::CodeNoError)
{
}

Error::Error(BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode):
        errorClass(errorClass),
        errorCode(errorCode)
{
}

bool Error::hasError()
{
    return (errorClass != BacnetErrorNS::ClassNoError);
}

void Error::setError(BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode)
{
    this->errorClass = errorClass;
    this->errorCode = errorCode;
}
