#include "error.h"

using namespace Bacnet;

Error::Error():
        errorClass(BacnetError::ClassNoError),
        errorCode(BacnetError::CodeNoError)
{
}

bool Error::hasError()
{
    return (errorClass != BacnetError::ClassNoError);
}

void Error::setError(BacnetError::ErrorClass errorClass, BacnetError::ErrorCode errorCode)
{
    this->errorClass = errorClass;
    this->errorCode = errorCode;
}
