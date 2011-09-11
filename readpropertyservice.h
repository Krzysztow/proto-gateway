#ifndef READPROPERTYSERVICE_H
#define READPROPERTYSERVICE_H

#include <QtCore>
#include <QList>

#include "bacnetcommon.h"//has to be included in this header file, since we need enumeration

class ReadPropertyServiceHandler
{
public:
    qint32 fromRaw(quint8 *servicePtr, quint16 length);
    void setDevice(void *device);
    QList<qint32> prepare();
    /** Shoud be used, when one of the handler acton is done (with specified internal id).
      \return returns true if the handler is ready to be executed.
      */
    bool finishAsynchAction(qint32 id, bool success);
    //! Informs if the all asynchronous service actions were finished (or if the ack is ready, due to some failure).
    bool isReady();
    /** Gets executed and returns the pointer to the ack message that is to be sent back.
      */
    void *execute();

private:
    ReadPropertyServiceHandler();

private:
    Bacnet::ObjectIdStruct _objId;
    BacnetProperty::Identifier _propId;
    quint32 _arrayIdx;

    void *_handledDevice;
    void *_handledObject;

    enum {
        NO_ARRAY_IDX_SPECIFIED = 0xffffffff
    };
};

#endif // READPROPERTYSERVICE_H
