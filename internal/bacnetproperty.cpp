#include "bacnetproperty.h"

#include "property.h"
#include "bacnetcommon.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

SimpleProperty::SimpleProperty(BacnetDataInterface *data):
    _data(data)
{
}

SimpleProperty::SimpleProperty(BacnetDataInterfaceShared &data):
    _data(data)
{
}

int SimpleProperty::getValue(BacnetDataInterfaceShared &data, quint32 propertyArrayIdx, Error *error, bool tryInstantly)
{
    Q_UNUSED(tryInstantly);
    //we are just simple property - there should be no arrayIdx specified
    if (propertyArrayIdx == ArrayIndexNotPresent) {
        data = _data;
    } else if (0 != error) {
        //index was specified
        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodePropertyIsNotAnArray);
    }
    return ::Property::ResultOk;
}

int SimpleProperty::setValue(BacnetDataInterfaceShared &data, quint32 propertyArrayIdx, Error *error)
{
    //we are just simple property - there should be no arrayIdx specified
    if (propertyArrayIdx == ArrayIndexNotPresent) {
        Q_ASSERT(data->typeId() == _data->typeId());
        if (data->typeId() == _data->typeId()) {
            data = _data;//due to being shared, if no one uses the old property, it will be deleted automatically.
        } else if (0 != error) {
            error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeInvalidDataType);
        }
    } else if (0 != error) {
        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodePropertyIsNotAnArray);
    }

    return ::Property::ResultOk;
}

////////////////////////////////////////////////////////////////
//////////////////ProxyInternalProperty/////////////////////////
////////////////////////////////////////////////////////////////

ProxyInternalProperty::ProxyInternalProperty(::Property *data, AppTags::BacnetTags bacnetType, QVariant::Type intenralType, InternalPropertyContainerSupport *parentSupporter):
    _data(data),
    _bacnetType(bacnetType),
    _internalType(intenralType),
    _parentSupporter(parentSupporter)
{
    Q_ASSERT(_bacnetType != AppTags::InvalidTag);
    Q_ASSERT(_bacnetType != AppTags::ASHRAE0);
    Q_ASSERT(_bacnetType != AppTags::ASHRAE1);
    Q_ASSERT(_bacnetType != AppTags::ASHRAE2);
    Q_ASSERT(_bacnetType != AppTags::LastAshraeTag);
    Q_ASSERT(_bacnetType != AppTags::ExtendedTagNumber);

    Q_ASSERT(_internalType != QVariant::Invalid);

    Q_CHECK_PTR(_parentSupporter);
}

int ProxyInternalProperty::getValue(BacnetDataInterfaceShared &data, quint32 propertyArrayIdx, Error *error, bool tryInstantly)
{
    //we are just singular property - there should be no arrayIdx specified
    if (propertyArrayIdx == ArrayIndexNotPresent) {
        if (_internalType == QVariant::Invalid) {
            qDebug("%s : property translation type is not specified", __PRETTY_FUNCTION__);
            Q_ASSERT(false);
            if (0 != error)
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
            return ::Property::ResultOk;//no asynchronous waiting
        }
        QVariant value(_internalType);
        qint32 ret(0);
        if (tryInstantly) {
            ret = _data->getValueInstant(&value);
            if (ret != ::Property::ResultOk) {
                qDebug("%s : Cannot read property instantly - ret code %d", __PRETTY_FUNCTION__, ret);
                if (0 != error)
                    error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
                return ::Property::NotSetYet;
            }
        } else {
            ret = _data->getValue(&value);
            if (ret < 0) {//something wrong
                qDebug("%s : Cannot access property - ret code %d", __PRETTY_FUNCTION__, ret);
                if (0 != error)
                    error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
                return ret;
            } else if (ret > 0) //being read asynchronously, that's ok
                return ret;
        }

        //here value has appropriate value set
        bool setOk(true);
        Bacnet::BacnetDataInterface *retProp = BacnetDefaultObject::createDataProperty(_bacnetType, &value, &setOk);
        if ((0 == retProp) || !setOk) {
            delete retProp;
            error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
            qDebug("Cannot allocate/find BacnetData or set property (%s) with value %s", setOk ? "true" : "false", qPrintable(value.toString()));
        }

        //we are done, set the data and return ret code (it's ok)
        data = BacnetDataInterfaceShared(retProp);
        return ret;
    }

    //index was specified
    error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodePropertyIsNotAnArray);
    return ::Property::ResultOk;
}

int ProxyInternalProperty::setValue(BacnetDataInterfaceShared &data, quint32 propertyArrayIdx, Error *error)
{
    //we are just singular property - there should be no arrayIdx specified
    if (propertyArrayIdx == ArrayIndexNotPresent) {
        if (_internalType == QVariant::Invalid) {
            qDebug("%s : property translation type is not specified", __PRETTY_FUNCTION__);
            Q_ASSERT(false);
            if (0 != error)
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
            return ::Property::ResultOk;//no asynchronous waiting
        }

        qint32 ret(0);
        QVariant value(data->toInternal());
        ret = _data->setValue(value);
        if (ret < ::Property::ResultOk) {
            qDebug("%s : Cannot set property instantly - ret code %d", __PRETTY_FUNCTION__, ret);
            if (0 != error)
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
        }

        //no matter what happened, return the same code. It's adequate to situation.
        return ret;
    }

    //index was specified
    error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodePropertyIsNotAnArray);
    return ::Property::ResultOk;
}

void ProxyInternalProperty::asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult)
{
    Q_ASSERT(property == _data);
    Q_UNUSED(property);

    if (0 != _parentSupporter)
        _parentSupporter->propertyAsynchActionFinished(asynchId, actionResult);
}

void ProxyInternalProperty::propertyValueChanged(Property *property)
{
    Q_ASSERT(property == _data);
    Q_UNUSED(property);

    if (0 != _parentSupporter)
        _parentSupporter->propertyValueChanged(this);
}

////////////////////////////////////////////////////////////////
///////////////////////ArrayProperty////////////////////////////
////////////////////////////////////////////////////////////////

#include "multipleasynchhelper.h"

ArrayProperty::ArrayProperty(InternalPropertyContainerSupport *parentSupporter):
    _asynchHelper(0),
    _parentSupporter(parentSupporter)
{
}

ArrayProperty::ArrayProperty(QList<BacnetProperty *> &data, InternalPropertyContainerSupport *parentSupporter):
    _data(data),
    _asynchHelper(0),
    _parentSupporter(parentSupporter)
{
}

ArrayProperty::~ArrayProperty()
{
    delete _asynchHelper;
    _asynchHelper = 0;
}

int ArrayProperty::indexOfProperty(BacnetProperty *property) const
{
    int idx = _data.indexOf(property);
    if (idx < 0)
        return ArrayIndexNotPresent;
    else
        return idx;
}

#warning "Remember to read object identifier separately!"

int ArrayProperty::getValue(BacnetDataInterfaceShared &data, quint32 propertyArrayIdx, Error *error, bool tryInstantly)
{
    if (propertyArrayIdx == ArrayIndexNotPresent) {
        //return entire array \note remember that some properties may be observers of internal ones
        QList<int> asynchIds;
        QList<BacnetDataInterfaceShared> readProperties;
        BacnetDataInterfaceShared readProperty;

        qint32 ret(0);
        QList<BacnetProperty*>::Iterator it = _data.begin();
        QList<BacnetProperty*>::Iterator itEnd = _data.begin();
        for (; it != itEnd; ++it) {
            ret = (*it)->getValue(readProperty, ArrayIndexNotPresent, error, tryInstantly);
            if (tryInstantly) {
                if (ret != ::Property::ResultOk) {
                    qDebug("%s : Cannot read property instantly from an array (%dth index) - ret code %d", __PRETTY_FUNCTION__, it - _data.begin(), ret);
                    if (0 != error)
                        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
                    return ::Property::NotSetYet;//no clean-up is needed, because all allocations are handled by QSharedPointers
                }
            } else {
                if (ret < 0) {//something wrong
                    qDebug("%s : Cannot access property from an array (%dth index) - ret code %d", __PRETTY_FUNCTION__, it - _data.begin(), ret);
                    if (0 != error)
                        error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
                    return ret;
                } else if (ret > 0) //being read asynchronously, that's ok
                    return ret;
            }

            if (ret > 0) {//read is done asynchronously
                asynchIds.append(ret);
            } else  {
                if (0 == asynchIds.count()) //Remember read values, only if we have a chance to read them all in this loop.
                    readProperties.append(readProperty);
                else
                    qDebug("%s : Some of the properties in an array were read asynchronously. Don't store values.", __PRETTY_FUNCTION__);
            }

            //just in case, clear theis value.
            readProperty.clear();
        }
        //iteration is done.
        if (0 == asynchIds.count()) {//we have no asynchronous action, the list contains all the necessary data.
            data = BacnetDataInterfaceShared(new BacnetArray(readProperties));
            return ::Property::ResultOk;
        } else if (1 == asynchIds.count()) //only one property was not read synchronously/isntantly
            return asynchIds.first();
        else {
            if (0 == _asynchHelper)
                _asynchHelper = new MultipleAsynchHelper();
            ret = _asynchHelper->addAsynchIds(asynchIds);
            Q_ASSERT(ret > 0);
            if (ret < 0)
                qDebug("%s : AsynchHelper returned %d", __PRETTY_FUNCTION__, ret);
            return ret;
        }


    } else if (propertyArrayIdx == 0) {
        //return length
        data = BacnetDataInterfaceShared(new UnsignedInteger(_data.count()));
    } else if (propertyArrayIdx <= _data.count()) {
        //read particular property
        Q_ASSERT(0 != _data[propertyArrayIdx - 1]);
        BacnetProperty *prop = _data[propertyArrayIdx - 1];
        if (0 == prop) {
            qDebug("%s : property (%d) in array not present!", __PRETTY_FUNCTION__, propertyArrayIdx);
            if (0 != error)
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
        } else
            return prop->getValue(data, ArrayIndexNotPresent, error, tryInstantly);
    } else {
        if (0 != error)
            error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeInvalidArrayIndex);
    }

    return ::Property::ResultOk;
}

int ArrayProperty::setValue(BacnetDataInterfaceShared &data, quint32 propertyArrayIdx, Error *error)
{
    if (propertyArrayIdx == ArrayIndexNotPresent) {
        /** we can't allow for writing entire array - imagine what would happen if one writes less parameters than there were originally?
            We could check for that, but for the time being, disallow it!
            \note Consider implementation.
          */
        if (0 != error)
            error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeWriteAccessDenied);
    } else if ( (propertyArrayIdx > 0) && (propertyArrayIdx < _data.count()) ) {
        //read particular property
        Q_ASSERT(0 != _data[propertyArrayIdx - 1]);
        BacnetProperty *prop = _data[propertyArrayIdx - 1];
        if (0 == prop) {
            qDebug("%s : property (%d) in array not present!", __PRETTY_FUNCTION__, propertyArrayIdx);
            if (0 != error)
                error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeOther);
        } else
            return prop->setValue(data, ArrayIndexNotPresent, error);
    } else {
        if (0 != error)
            error->setError(BacnetErrorNS::ClassProperty, BacnetErrorNS::CodeInvalidArrayIndex);
    }

    return ::Property::ResultOk;
}

void ArrayProperty::propertyAsynchActionFinished(int asynchId, ::Property::ActiontResult result,
                                                 BacnetProperty *subProperty, BacnetProperty *mainProperty,
                                                 BacnetObject *parentObject, BacnetDeviceObject *deviceObject)
{
    Q_CHECK_PTR(subProperty);
    Q_ASSERT(0 == mainProperty);
    Q_UNUSED(mainProperty);
    Q_ASSERT(0 == parentObject);
    Q_UNUSED(parentObject);
    Q_ASSERT(0 == deviceObject);
    Q_UNUSED(deviceObject);
    Q_ASSERT(_data.contains(subProperty));

    if (0 != _asynchHelper) {
        ::MultipleAsynchHelper::DoneState multiState;
        ::Property::ActiontResult multiResult;
        int multiId;
        multiState = _asynchHelper->asynchIdDone(asynchId, result, &multiId, &multiResult);
        if (::MultipleAsynchHelper::Accepted == multiState)//we have nothing to do yet, not all actions were finished.
            return;
        else if (::MultipleAsynchHelper::Finished == multiState) {
            asynchId = multiId;
            result = multiResult;
        }
    }

    if (0 != _parentSupporter) {
        _parentSupporter->propertyAsynchActionFinished(asynchId, result, subProperty, this);
    }
}

void Bacnet::ArrayProperty::propertyValueChanged(BacnetProperty *property, ArrayProperty *arrayProperty, BacnetObject *parentObject, BacnetDeviceObject *deviceObject)
{
    Q_CHECK_PTR(property);
    Q_ASSERT(0 == arrayProperty);
    Q_UNUSED(arrayProperty);
    Q_ASSERT(0 == parentObject);
    Q_UNUSED(parentObject);
    Q_ASSERT(0 == deviceObject);
    Q_UNUSED(deviceObject);
    Q_ASSERT(_data.contains(property));

    if (0 != _parentSupporter) {
        _parentSupporter->propertyValueChanged(property, this);
    }
}

