#ifndef EXTERNALOBJECTSHANDLER_H
#define EXTERNALOBJECTSHANDLER_H

#include <QtCore>
#include <QObject>

#include "propertyowner.h"
#include "property.h"
#include "bacnetexternalobjects.h"
#include "bacnetcommon.h"
#include "bacnetinternaladdresshelper.h"


namespace Bacnet {

    class BacnetReadPropertyAck;
    class BacnetTSM2;
    class ExternalConfirmedServiceHandler;
    class Error;
    class BacnetApplicationLayerHandler;

    class ExternalObjectsHandler:
            public QObject,
            public PropertyOwner
    {
        Q_OBJECT;
    public:
        ExternalObjectsHandler(BacnetApplicationLayerHandler *appLayer);

        void addMappedProperty(Property *property, quint32 objectId,
                               BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx,
                               quint32 deviceId = BacnetObjectTypeNS::Undefined,
                               BacnetExternalObjects::ReadAccessType type = BacnetExternalObjects::Access_COV_Uninitialized);

        BacnetExternalObjects::ExternalRoutingElement &routingEntry(::Property *property, bool *found = 0);

        void handleResponse(ExternalConfirmedServiceHandler *act, BacnetReadPropertyAck &rp);
        void handleResponse(ExternalConfirmedServiceHandler *act, bool ok);//all the simple acks come here.
        void handleError(ExternalConfirmedServiceHandler *act, Error &error);
        void handleAbort(ExternalConfirmedServiceHandler *act,  quint8 abortReason);

        bool isRegisteredAddress(InternalAddress &address);
        void addRegisteredAddress(InternalAddress &address);
        void removeRegisteredAddress(InternalAddress &address);
        BacnetAddress oneOfAddresses();

    public://overridden from PropertyOwner
        int getPropertyRequested(PropertySubject *toBeGotten);
        virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);

        /** This function shouldn't be ever called. This owner should contain only external objects, which are
          handled by themselves.
          */
        virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);

        /** This function is not used, since all the objects here are PropertySubjects and will not be changed
          from within CDM.
         */
        virtual void propertyValueChanged(Property *property);

    private:
        int readProperty(BacnetExternalObjects::ExternalRoutingElement &readElement, PropertySubject *property);


    private:
        QVector<BacnetExternalObjects::ExternalRoutingElement> _routingTable;

        enum RequestType {
            RequestWrite,
            RequestRead,
            RequestCOV
        };

        struct RequestInfo {
            int asynchId;
            RequestType reqType;
            ::PropertySubject *concernedProperty;
        };

        QMap<ExternalConfirmedServiceHandler*, RequestInfo> _bacnetPendingRequests;

        BacnetApplicationLayerHandler *_appLayer;
        QList<InternalAddress> _registeredAddresses;
    };

}

#endif // EXTERNALOBJECTSHANDLER_H
