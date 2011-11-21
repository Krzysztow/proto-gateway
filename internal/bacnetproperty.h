#ifndef BACNETPROPERTY_H
#define BACNETPROPERTY_H

#include <QSharedPointer>
#include "bacnetcommon.h"
#include "bacnetdata.h"
#include "bacnetconstructeddata.h"
#include "error.h"

class Property;//internal property predeclaration

namespace Bacnet {

/**This class provides basic interface for all types of properties that are available in bacnet module.
  Those are:
  # \sa SimpleProperty
  # \sa ProxyInternalProperty
  # \sa ArrayProperty
  */

class BacnetProperty
{
public:
    virtual ~BacnetProperty(){}

    virtual int getValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0, bool tryInstantly = true) = 0;
    virtual int setValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0) = 0;
};

/**
  This class is meant to hold non-alterable bacnet data.
  */

class SimpleProperty:
        public BacnetProperty
{
public:
    SimpleProperty(BacnetDataInterface *data);
    SimpleProperty(BacnetDataInterfaceShared &data);

public://overriden from Bacnet::Property
    virtual int getValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0, bool tryInstantly = true);
    virtual int setValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0);

private:
    BacnetDataInterfaceShared _data;
};

}

/**
  This class is meant to keep CDM data. It should link to only one Internal::Property.
    \todo Consider creating ProxutInternalComplexProperty for bacnet datatypes that might consist of few properties.
  */
#include "propertyowner.h"
#include "internalpropertycontainersupport.h"

namespace Bacnet {

class ProxyInternalProperty:
        public BacnetProperty,
        public ::PropertyOwner
{
public:
    /**
      Proxy constructor. DataType::DataType type has to be specified, so that we know what type internal property is to be converted to.
      */
    ProxyInternalProperty(::Property *data, AppTags::BacnetTags bacnetType, QVariant::Type intenralType, InternalPropertyContainerSupport *parentSupporter);

public://overriden from Bacnet::Property
    virtual int getValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0, bool tryInstantly = true);
    virtual int setValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0);

public://owerriden from PropertyOwner
    //! \todo this one is not implemented yet!
    virtual int getPropertyRequested(PropertySubject *toBeGotten) {Q_UNUSED(toBeGotten); return ::Property::UnknownError;}
    //! \todo this one is not implemented yet!
    virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value) {Q_UNUSED(toBeSet); Q_UNUSED(value); return ::Property::UnknownError;}

    //! Invoked by stored PropertyObserver when asynchronous read/write is finished.
    virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);
    //! Invoked by stored PropertyObserver when observer value got changed.
    virtual void propertyValueChanged(Property *property);

private:
    //! \todo We know that we use only BacnetPropertyListener here.
    ::Property *_data;
    AppTags::BacnetTags _bacnetType;
    QVariant::Type _internalType;

    InternalPropertyContainerSupport *_parentSupporter;
};

/**
  This class is meant to be used as bacnet array/list holder. It stores Bacnet::Properties, meaning it can hold both \sa SimpleProperty
  as well as \sa ProxyInternalProperty.
  */
}
#include "internalpropertycontainersupport.h"

class MultipleAsynchHelper;

namespace Bacnet {

class ArrayProperty:
        public BacnetProperty,
        public InternalPropertyContainerSupport
{
public:
    ArrayProperty(InternalPropertyContainerSupport *parentSupporter);
    ArrayProperty(QList<BacnetProperty*> &data, InternalPropertyContainerSupport *parentSupporter);
    virtual ~ArrayProperty();

    int indexOfProperty(BacnetProperty *property) const;

public://overriden from Bacnet::Property
    virtual int getValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0, bool tryInstantly = true);
    virtual int setValue(BacnetDataInterfaceShared &data, quint32 BacnetPropertyArrayIdx = ArrayIndexNotPresent, Error *error = 0);

public://overriden from InternalPropertyContainerSupport
    virtual void propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                              BacnetProperty *subProperty = 0, BacnetProperty *mainProperty = 0,
                                              BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0);

    virtual void propertyValueChanged(BacnetProperty *property = 0, ArrayProperty *arrayProperty = 0,
                                      BacnetObject *parentObject = 0, BacnetDeviceObject *deviceObject = 0);

private:
    //! \todo We know that we use only BacnetPropertyListener here.
    QList<BacnetProperty*> _data;
    MultipleAsynchHelper *_asynchHelper;

    InternalPropertyContainerSupport *_parentSupporter;
};

}

#endif // BACNETPROPERTY_H
