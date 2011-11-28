#ifndef ERROR_H
#define ERROR_H

#include "bacnetcommon.h"
namespace Bacnet {

    class Error {
    public:
        Error(BacnetServicesNS::BacnetErrorChoice errorChoice/* = BacnetServicesNS::AcknowledgeAlarm*/);
        Error(BacnetServicesNS::BacnetErrorChoice errorChoice, BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode);

        bool hasError();
        void setError(BacnetErrorNS::ErrorClass errorClass, BacnetErrorNS::ErrorCode errorCode);

        quint8 errorChoice();
        void setErrorChoice(BacnetServicesNS::BacnetErrorChoice  errorChoice);

        qint32 appPartFromRaw(quint8 *errorData, quint16 bufferLength);
        qint32 appPartToRaw(quint8 *startPtr, quint16 bufferLength);

    public:
        BacnetServicesNS::BacnetErrorChoice _errorChoice;
        BacnetErrorNS::ErrorClass errorClass;
        BacnetErrorNS::ErrorCode errorCode;
    };

}

#endif // ERROR_H
