#ifndef SUBSCRIBECOVSERVICEDATA_H
#define SUBSCRIBECOVSERVICEDATA_H

#include <QtCore>

#include "bacnetservicedata.h"
#include "bacnetcommon.h"

namespace Bacnet {

    class SubscribeCOVServiceData:
            public BacnetServiceData
    {
    public:
        SubscribeCOVServiceData();

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        inline bool isConfirmedNotificationPresent() {return flags & IssueConfNotPresent;}
        inline void setConfirmedNotificationPresent() {flags |= IssueConfNotPresent;}
        inline void clearConfirmedNotificationPresent() {flags &= (~IssueConfNotPresent);}

        inline bool isLifetimePresent() {return flags & LifetimePresent;}
        inline void setLifetimePresent() {flags |= LifetimePresent;}
        inline void clearLifetimePresent() {flags &= (~LifetimePresent);}

    public://is there any reason we should make it private?
        quint32 _subscriberProcId;
        ObjectIdStruct _monitoredObjectId;
        bool _issueConfNotification;
        quint32 _lifetime;

    private:
        enum {
            IssueConfNotPresent = 0x01,
            LifetimePresent     = 0x02,

            AllFlags = IssueConfNotPresent | LifetimePresent
        };
        quint8 flags;
    };

}
#endif // SUBSCRIBECOVSERVICEDATA_H
