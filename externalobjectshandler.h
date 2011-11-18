#ifndef EXTERNALOBJECTSHANDLER_H
#define EXTERNALOBJECTSHANDLER_H

#include <QtCore>

#include "propertyowner.h"
#include "property.h"
#include "bacnetexternalobjects.h"
#include "bacnetcommon.h"
#include "bacnetinternaladdresshelper.h"


namespace Bacnet {

    class BacnetReadPropertyAck;
    class BacnetTSM2;
    class BacnetConfirmedServiceHandler;
    class Error;

    class ExternalObjectsHandler:
            public PropertyOwner
    {
    public:
        ExternalObjectsHandler(BacnetTSM2 *tsm);

        void addMappedProperty(Property *property, quint32 objectId,
                               BacnetProperty::Identifier propertyId, quint32 propertyArrayIdx,
                               quint32 deviceId = BacnetObjectType::Undefined,
                               BacnetExternalObjects::ReadAccessType type = BacnetExternalObjects::Access_COV_Uninitialized);

        BacnetExternalObjects::ExternalRoutingElement &routingEntry(::Property *property, bool *found = 0);

        void handleResponse(BacnetConfirmedServiceHandler *act, BacnetReadPropertyAck &rp);
        void handleResponse(BacnetConfirmedServiceHandler *act, bool ok);//all the simple acks come here.
        void handleError(BacnetConfirmedServiceHandler *act, Error &error);
        void handleAbort(BacnetConfirmedServiceHandler *act,  quint8 abortReason);

        bool isRegisteredAddress(InternalAddress &address);
        void addRegisteredAddress(InternalAddress &address);
        void removeRegisteredAddress(InternalAddress &address);

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

        QMap<BacnetConfirmedServiceHandler*, RequestInfo> _bacnetPendingRequests;

        BacnetTSM2 *_tsm;
        QList<InternalAddress> _registeredAddresses;
    };

}

#endif // EXTERNALOBJECTSHANDLER_H
