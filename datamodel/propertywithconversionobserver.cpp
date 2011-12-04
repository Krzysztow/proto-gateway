#include "propertywithconversionobserver.h"

#include "propertysubject.h"
#include "propertyconverter.h"
#include "propertyconvertersowner.h"

using namespace DataModelNS;

PropertyWithConversionObserver::PropertyWithConversionObserver(PropertyOwner *container, PropertySubject *property, PropertyConverter *converter):
    PropertyObserver(container, property),
    _converter(converter)
{

}

int PropertyWithConversionObserver::getValue(QVariant *outValue)
{
    if (0 == outValue)
        return PropertyObserver::getValue(outValue);

    QVariant &out = *outValue;
    QVariant internalToBeConverted(_property->type());
    int ret = PropertyObserver::getValue(&internalToBeConverted);
    if (ret > 0)
        return ret;
    else if (Property::ResultOk == ret) {
        if (myConverter()->convertFromInternal(internalToBeConverted, out))
            return Property::ResultOk;
        else {
            qDebug("%s : Conversion went wrong!", __PRETTY_FUNCTION__);
            return Property::TypeMismatch;
        }
    }

    return ret;
}

PropertyConverter *PropertyWithConversionObserver::myConverter()
{
    if (0 == _converter)
        return PropertyConvertersOwner::instance()->universalConverter();
    return _converter;
}

int PropertyWithConversionObserver::setValue(QVariant &inValue)
{
    QVariant internalConverted(_property->type());//internal converted is not converted yet
    int ret = PropertyObserver::getValueInstant(&internalConverted);//get the stored value, since some types of conversion depend on that (like bit masking)
    if (ret < Property::ResultOk)   //if got error, pass it further
        return ret;
    else if (ret > Property::ResultOk)//if was asynchronous - > shouldn't ever happen! We wan't value instantly.
        return Property::UnknownError;

    if (!myConverter()->convertToInternal(internalConverted, inValue))// internal converted now represents converted value
        return Property::TypeMismatch;

    return PropertyObserver::setValue(internalConverted);
}

int PropertyWithConversionObserver::getValueInstant(QVariant *outValue)
{
    Q_ASSERT(_property);

    if (0 == outValue)
        return PropertyObserver::getValueInstant(outValue);

    QVariant &out = *outValue;
    QVariant valueToBeConverted(_property->type());
    int ret = PropertyObserver::getValueInstant(&valueToBeConverted);
    if (ret < Property::ResultOk)
        return ret;
    else if (ret > Property::ResultOk)
        return Property::UnknownError;//we wanted it instantly

    if (myConverter()->convertFromInternal(valueToBeConverted, out))
        return Property::ResultOk;

    return Property::UnknownError;
}


//#define PROP_CONV_TEST
#ifdef PROP_CONV_TEST
#include "cdm.h"
#include <QDebug>
int main()
{
    DataModel::instance()->startFactory();



    DataModel *dm = DataModel::instance();
    PropertySubject *subject = dm->createPropertySubject(1, QVariant::Bool);

    PropertyObserver *observer = dm->createPropertyObserver(1);
    PropertyWithConversionObserver *convObserver = new PropertyWithConversionObserver(0, subject, new PropertyScalerConverter(1000));


    DataModel::instance()->stopFactory();

    QVariant value((bool)true);
    subject->setValue(value);

    QVariant convValue(QVariant::Int);
    convObserver->getValue(&convValue);

    return 0;
}

#undef PROP_CONV_TEST
#endif
