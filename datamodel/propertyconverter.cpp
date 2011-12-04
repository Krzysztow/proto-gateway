#include "propertyconverter.h"

using namespace DataModelNS;

bool PropertyUniversalConverter::convertFromInternal(QVariant &internalValue, QVariant &externalValue)
{
    return convertFromOneToTwohelper(internalValue, externalValue);
}

bool PropertyUniversalConverter::convertToInternal(QVariant &internalValue, QVariant &externalValue)
{
    return convertFromOneToTwohelper(externalValue, internalValue);
} // namespace DataModel

bool PropertyUniversalConverter::convertFromOneToTwohelper(QVariant &one, QVariant &two)
{
    Q_ASSERT(!one.isNull());
    Q_ASSERT(!two.isNull());

    if (one.isNull() || !one.isValid() || //the property which value we take is not set, or has no type
            !two.isValid()) {                         // the property to be set has no type
        qDebug("%s : Property null or not valid %d, %d, %d", __PRETTY_FUNCTION__, one.isNull(), one.isValid(), two.isValid());
        Q_ASSERT(false);
        return false;
    }

    QVariant temp(one);
    bool ok = temp.convert(two.type());
    if (ok)
        two = temp;

    return ok;
}

////////////////////////////////////////////////////////////////

PropertyScalerConverter::PropertyScalerConverter(float multiplyFactorFromInternalToExternal):
    _multiplyFromInternalToExternal(multiplyFactorFromInternalToExternal)
{
    Q_ASSERT(0 != _multiplyFromInternalToExternal);
    if (0 == _multiplyFromInternalToExternal)
        _multiplyFromInternalToExternal = 1;
}


bool PropertyScalerConverter::convertFromInternal(QVariant &internalValue, QVariant &externalValue)
{
    return fromOneToTwo_helper(internalValue, externalValue, true);
}

bool PropertyScalerConverter::convertToInternal(QVariant &internalValue, QVariant &externalValue)
{
    return fromOneToTwo_helper(externalValue, internalValue, false);
}

QList<QVariant::Type> PropertyScalerConverter::allowableTypes()
{
    return QList<QVariant::Type>() << QVariant::Bool <<
                                      QVariant::Int << QVariant::LongLong <<
                                      QVariant::UInt << QVariant::ULongLong <<
                                      QVariant::Double << (QVariant::Type)QMetaType::Float;
}

bool PropertyScalerConverter::fromOneToTwo_helper(QVariant &one, QVariant &two, bool doMultiply)
{
    Q_ASSERT(!one.isNull());
    Q_ASSERT(!two.isNull());

    if (one.isNull() || !one.isValid() || //the property which value we take is not set, or has no type
            !two.isValid()) {                         // the property to be set has no type
        qDebug("%s : Property null or not valid %d, %d, %d", __PRETTY_FUNCTION__, one.isNull(), one.isValid(), two.isValid());
        Q_ASSERT(false);
        return false;
    }

    bool ok;
    double tempVal = one.toDouble(&ok);
    if (!ok)
        return ok;

    Q_ASSERT(_multiplyFromInternalToExternal != 0);
    if (doMultiply)
        tempVal *= _multiplyFromInternalToExternal;
    else
        tempVal /= _multiplyFromInternalToExternal;

    QVariant outVal(tempVal);
    ok = outVal.convert(two.type());
    if (ok)
        two = outVal;
    return ok;
}
