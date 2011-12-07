#include "error.h"

#include "bacnetcoder.h"
#include "bacnettagparser.h"

using namespace Bacnet;

Error::Error(BacnetServicesNS::BacnetErrorChoice errorChoice):
    _errorChoice(errorChoice),
    errorClass(BacnetErrorNS::ClassNoError),
    errorCode(BacnetErrorNS::CodeNoError)
{
}

Error::Error(BacnetServicesNS::BacnetErrorChoice errorChoice, BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode):
    _errorChoice(errorChoice),
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

qint32 Error::appPartFromRaw(quint8 *errorData, quint16 bufferLength)
{
    BacnetTagParser bParser(errorData, bufferLength);
    bool convOk;

    qint32 total(0);
    qint32 ret(0);

    ret = bParser.parseNext();
    errorClass = (BacnetErrorNS::ErrorClass)bParser.toEumerated(&convOk);
    if (!convOk || (ret <= 0) || !bParser.isApplicationTag(AppTags::Enumerated)) {
        qDebug("%s : Cannot parse the error class", __PRETTY_FUNCTION__);
        return BacnetErrorNS::CodeOther;
    }

    total += ret;
    ret = bParser.parseNext();
    errorCode = (BacnetErrorNS::ErrorCode)bParser.toEumerated(&convOk);
    if (!convOk || (ret <= 0) || !bParser.isApplicationTag(AppTags::Enumerated)) {
        qDebug("%s : Cannot parse the error code", __PRETTY_FUNCTION__);
        return BacnetErrorNS::CodeOther;
    }
    total += ret;

    return total;
}

qint32 Error::appPartToRaw(quint8 *startPtr, quint16 bufferLength)
{
    qint32 ret(0);
    quint8 *actualPtr(startPtr);

    ret = BacnetCoder::uintToRaw(actualPtr, bufferLength, errorClass, false, AppTags::Enumerated);
    if (ret < 0)
        return ret;

    actualPtr += ret;
    bufferLength -= ret;
    ret = BacnetCoder::uintToRaw(actualPtr, bufferLength, errorCode, false, AppTags::Enumerated);
    if (ret < 0)
        return ret;

    actualPtr += ret;

    return actualPtr - startPtr;
}

quint8 Bacnet::Error::errorChoice()
{
    return _errorChoice;
}

void Error::setErrorChoice(BacnetServicesNS::BacnetErrorChoice errorChoice)
{
    _errorChoice = errorChoice;
}
