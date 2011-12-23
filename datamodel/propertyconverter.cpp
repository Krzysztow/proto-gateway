#include "propertyconverter.h"

using namespace DataModelNS;

PropertyUniversalConverter::~PropertyUniversalConverter()
{
}


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
    Q_ASSERT(two.isValid());

    if (one.isNull() ||         //the property which value we take is not set, or has no type
            !two.isValid()) {   // the property to be set has no type
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
    Q_ASSERT(!one.isNull());//value has to be set and has info type
    Q_ASSERT(two.isValid());//we need type info

    if (one.isNull() ||             // the property which value we take is not set, or has no type
            !two.isValid()) {       // the property to be set has no type
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

///////////////////////////////////////
///////PropertyBitmaskConverter////////
///////////////////////////////////////

DataModelNS::PropertyBitmaskConverter::PropertyBitmaskConverter(QBitArray &mask):
    _bitmaskFromInternal(mask)
{
}

template <class T>
QBitArray bitArrayFromType(T value) {
    Q_ASSERT(sizeof(T) <= sizeof(quint64));
    if (sizeof(T) > sizeof(quint64))
        return QBitArray();
    quint64 mask = 0x0000000000000001;
    QBitArray result(8 * sizeof(T), false);
    for (uint i = 0; i < 8 * sizeof (T); ++i) {
        if (mask & value)
            result.setBit(i, true);
        mask <<= 1;
    }

    return result;
}

template <class T>
T typeFromBitArray(QBitArray &array) {
    Q_ASSERT(sizeof(T) <= sizeof(quint64));
    quint64 mask = 0x0000000000000001;
    T resultVector(0);
    for (uint i = 0; i < 8 * sizeof (T); ++i) {
        if (array.testBit(i))
            resultVector |= mask;
        mask <<= 1;
    }
    return resultVector;
}

#define CASE_RETURN_VARIANT_TO_BIT_ARRAY(varType,convMethod,type) \
    case (varType): {                   \
    type v = variant.convMethod(&ok); \
    Q_ASSERT(ok);                       \
    return bitArrayFromType<type>(v);  \
    }


QBitArray variantToBitArray(QVariant &variant, bool &ok)
{
    ok = true;
    switch (variant.type()) {
    case (QVariant::Bool):{
        bool v = variant.toBool();
        return bitArrayFromType<bool>(v);
    }
        CASE_RETURN_VARIANT_TO_BIT_ARRAY(((QVariant::Type)QMetaType::UChar),toUInt,uchar);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY(((QVariant::Type)QMetaType::Char),toInt,char);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY(((QVariant::Type)QMetaType::UShort),toUInt,ushort);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY(((QVariant::Type)QMetaType::Short),toInt,short);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY((QVariant::UInt),toUInt,uint);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY((QVariant::ULongLong),toULongLong,qulonglong);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY((QVariant::Int),toInt,int);
        CASE_RETURN_VARIANT_TO_BIT_ARRAY((QVariant::LongLong),toLongLong,qlonglong);
    case (QVariant::BitArray):{
        QBitArray v = variant.toBitArray();
        return v;
    }
    default:
        ok = false;
        Q_ASSERT(ok);
        return QBitArray();
    }
}

#define TYPE_CONV_FROM_ARRAY(type) variant.setValue((type)typeFromBitArray<type>(array)); break;
bool variantFromBitArray(QVariant &variant, QBitArray &array) {
    switch (variant.type()) {
    case (QVariant::Bool):
        TYPE_CONV_FROM_ARRAY(bool);
    case ((QVariant::Type)QMetaType::UChar):
        TYPE_CONV_FROM_ARRAY(uchar);
    case ((QVariant::Type)QMetaType::Char):
        TYPE_CONV_FROM_ARRAY(char);
    case ((QVariant::Type)QMetaType::UShort):
        TYPE_CONV_FROM_ARRAY(ushort);
    case ((QVariant::Type)QMetaType::Short):
        TYPE_CONV_FROM_ARRAY(short);
    case (QVariant::UInt):
        TYPE_CONV_FROM_ARRAY(uint);
    case (QVariant::ULongLong):
        TYPE_CONV_FROM_ARRAY(qulonglong);
    case (QVariant::Int):
        TYPE_CONV_FROM_ARRAY(int);
    case (QVariant::LongLong):
        TYPE_CONV_FROM_ARRAY(qlonglong);
    case (QVariant::BitArray):
        variant.setValue(array);
        break;
    default:
        return false;
    }

    return true;
}

bool PropertyBitmaskConverter::convertFromInternal(QVariant &internalValue, QVariant &externalValue)
{
    return applyMaskWithInValues_helper(internalValue, externalValue);
}

bool PropertyBitmaskConverter::convertToInternal(QVariant &internalValue, QVariant &externalValue)
{
    return applyMaskWithInValues_helper(externalValue, internalValue);
}

const QBitArray &PropertyBitmaskConverter::mask()
{
    return _bitmaskFromInternal;
}

QList<QVariant::Type> PropertyBitmaskConverter::allowableTypes()
{
    return QList<QVariant::Type>() << QVariant::Bool <<
                                      QVariant::Int << QVariant::LongLong <<
                                      QVariant::UInt << QVariant::ULongLong <<
                                      QVariant::BitArray;
}

#include <QDebug>
#include "helpercoder.h"

bool PropertyBitmaskConverter::applyMaskWithInValues_helper(QVariant &inVariant, QVariant &outVariant)
{
    bool ok(true);
    QBitArray inBitRepres = variantToBitArray(inVariant, ok);
    qDebug()<<"In value \t"<<inBitRepres;
    if (!ok)
        return ok;
    QBitArray outBitRepres = variantToBitArray(outVariant, ok);
    qDebug()<<"Out value \t"<<outBitRepres;
    if (!ok)
        return ok;
    Q_ASSERT(!_bitmaskFromInternal.isNull());
    QBitArray maskCopy = _bitmaskFromInternal;
    if (outBitRepres.count() != 0)
        maskCopy.truncate(outBitRepres.size());
    else
        outBitRepres.resize(maskCopy.count());
    inBitRepres.truncate(maskCopy.count());
    qDebug()<<"Mask (mod)\t"<<maskCopy;

    //! \todo optimize and beautify it
    if (inVariant.type() == QVariant::Bool)
        inBitRepres = maskCopy;
    else
        inBitRepres &= (maskCopy);//get only masked bits
    outBitRepres &= (~maskCopy);//clear mask bits
    outBitRepres |= inBitRepres;//set relevant bits from mask
    qDebug()<<"Out res\t"<<outBitRepres;

    return variantFromBitArray(outVariant, outBitRepres);
}



//#define PROP_CONV_TEST
#ifdef PROP_CONV_TEST

#define ASSERT_EXTERNAL_EQUALS_ASSERTER(asserter) Q_ASSERT(external.type() == asserter.type()); Q_ASSERT(external == asserter);
int main()
{
    QVariant asserter;

    QVariant internal;
    internal.setValue((quint8)0x02);
    QVariant external;
    external.setValue((quint16)0x04);

    bool ok(false);
    QVariant bMask;
    bMask.setValue((quint16)0xf2);
    QBitArray bitmask(variantToBitArray(bMask, ok));

    PropertyBitmaskConverter propConverter(bitmask);

    //converting from external value 0x0004 to 0x0006 (because masked 0x02 by 0xf2 is 0x02). Type can't be changed
    ok = propConverter.convertFromInternal(internal, external);
    asserter.setValue((quint16)0x06);
    ASSERT_EXTERNAL_EQUALS_ASSERTER(asserter);

    //converting back from external 0x0006 to 0x0004 (because masked 0x01 bu 0x0f is 0x00). Type cant change.
    internal.setValue((quint8)0x01);
    ok = propConverter.convertFromInternal(internal, external);
    asserter.setValue((quint16)0x04);
    ASSERT_EXTERNAL_EQUALS_ASSERTER(asserter);

    QBitArray externalBits(24, false);
    externalBits.setBit(2, true);
    external.setValue(externalBits);
    internal.setValue((quint8)0x01);

    //external is 0x0 00 04 and is ORed with masked 0x01, which is x00 -> no change!
    QBitArray asserterBits(externalBits);
    asserter.setValue(asserterBits);
    propConverter.convertFromInternal(internal, external);
    ASSERT_EXTERNAL_EQUALS_ASSERTER(asserter);

    internal.setValue((quint8)0x03);
    //exteranal is 0x0 00 04 and is ORed with masked 0x03, which is 0x02 -> should be changed to 0x0 00 06
    asserterBits.fill(false);
    asserterBits.setBit(1, true);
    asserterBits.setBit(2, true);
    asserter.setValue(asserterBits);
    propConverter.convertFromInternal(internal, external);
    ASSERT_EXTERNAL_EQUALS_ASSERTER(asserter);
    //check

}

#endif
#undef PROP_CONV_TEST



