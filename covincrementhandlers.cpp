#include "covincrementhandlers.h"

#include "bacnetprimitivedata.h"

using namespace Bacnet;

template <class T, class V>
CovIncrementHandler<T, V>::CovIncrementHandler(T &covIncremenet):
    _lastInformedValue(0),
    _covIncrement(covIncremenet)
{
}

template <class T, class V>
CovIncrementHandler<T, V>::CovIncrementHandler():
    _lastInformedValue(0)
{
    _covIncrement.setValue(0);
}

template <class T, class V>
void CovIncrementHandler<T, V>::comparison_helper(V dataValue)
{
    if (_lastInformedValue == dataValue)
        _state = Equal;
    else if ( (dataValue >= _lastInformedValue) ) {
        if (_lastInformedValue + _covIncrement.value() > dataValue)
            _state = EqualWithinInvrement;
        else
            _state = NotEqualWithinIncrement;
    }
    else if (dataValue <= _lastInformedValue) {
        if (_lastInformedValue - _covIncrement.value() < dataValue)
            _state = EqualWithinInvrement;
        else
            _state = NotEqualWithinIncrement;
    }
    //update remembered value
    if (EqualWithinInvrement != _state)
        _lastInformedValue = dataValue;
}

template <class T, class V>
void CovIncrementHandler<T, V>::visit(Real &data)
{
    comparison_helper(data.value());
}

template <class T, class V>
void CovIncrementHandler<T, V>::visit(Double &data)
{
    comparison_helper(data.value());
}

template <class T, class V>
void CovIncrementHandler<T, V>::visit(UnsignedInteger &data)
{
    comparison_helper(data.value());
}

template <class T, class V>
void CovIncrementHandler<T, V>::visit(SignedInteger &data)
{
    comparison_helper(data.value());
}

template <class T, class V>
void CovIncrementHandler<T, V>::visit(BacnetDataInterface &data)
{
    Q_UNUSED(data);
    qDebug("CovIncrementHandler<T, V>::visit(BacnetDataInterface&) always marks as not equal!");
    //! \todo Or maybe we should compare qvariants?
    _state = NotEqualWithinIncrement;
}

template <class T, class V>
qint32 CovIncrementHandler<T, V>::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    return _covIncrement.toRaw(ptrStart, buffLength);
}

template <class T, class V>
qint32 CovIncrementHandler<T, V>::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    return _covIncrement.toRaw(ptrStart, buffLength, tagNumber);
}

template <class T, class V>
qint32 CovIncrementHandler<T, V>::fromRaw(BacnetTagParser &parser)
{
    return _covIncrement.fromRaw(parser);
}

template <class T, class V>
qint32 CovIncrementHandler<T, V>::fromRaw(BacnetTagParser &parser, quint8 tagNum)
{
    return _covIncrement.fromRaw(parser, tagNum);
}


namespace Bacnet {//otherwise compilator complains there are specializations in a differenet namespace

template <>
void CovIncrementHandler<Real, float>::visit(BacnetDataInterface &data)
{
    QVariant dataValue = data.toInternal();
    if (dataValue.canConvert(QVariant::Double)) {
        bool ok;
        float value = dataValue.toDouble(&ok);
        Q_ASSERT(ok);
        if (ok)
            comparison_helper(value);
    }
}

//template <>
//void CovIncrementHandler<double>::visit(BacnetDataInterface &data)
//{
//    QVariant dataValue = data.toInternal();
//    if (dataValue.canConvert(QVariant::Double)) {
//        bool ok;
//        double value = dataValue.toDouble(&ok);
//        Q_ASSERT(ok);
//        if (ok)
//            comparison_helper(value);
//    }
//}

//template <>
//void CovIncrementHandler<qint32>::visit(BacnetDataInterface &data)
//{
//    QVariant dataValue = data.toInternal();
//    if (dataValue.canConvert(QVariant::Int)) {
//        bool ok;
//        qint32 value = dataValue.toInt(&ok);
//        Q_ASSERT(ok);
//        if (ok)
//            comparison_helper(value);
//    }
//}

//template <>
//void CovIncrementHandler<quint32>::visit(BacnetDataInterface &data)
//{
//    QVariant dataValue = data.toInternal();
//    if (dataValue.canConvert(QVariant::Int)) {
//        bool ok;
//        quint32 value = dataValue.toInt(&ok);
//        Q_ASSERT(ok);
//        if (ok)
//            comparison_helper(value);
//    }
//}

}

////avoiding linking problems
template class CovIncrementHandler<Real, float>;
//template class CovIncrementHandler<double>;
//template class CovIncrementHandler<quint32>;
//template class CovIncrementHandler<qint32>;
