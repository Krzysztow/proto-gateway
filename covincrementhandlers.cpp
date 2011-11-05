#include "covincrementhandlers.h"

#include "bacnetprimitivedata.h"

using namespace Bacnet;

template <class T>
CovIncrementHandler<T>::CovIncrementHandler(T &covIncremenet, T &lastValue):
    _lastInformedValue(lastValue),
    _covIncrement(covIncremenet)
{
}

template <class T>
void CovIncrementHandler<T>::comparison_helper(T dataValue)
{
    if (_lastInformedValue == dataValue)
        _state = Equal;
    else if ( (dataValue >= _lastInformedValue) ) {
        if (_lastInformedValue + _covIncrement > dataValue)
            _state = EqualWithinInvrement;
        else
            _state = NotEqualWithinIncrement;
    }
    else if (dataValue <= _lastInformedValue) {
        if (_lastInformedValue - _covIncrement < dataValue)
            _state = EqualWithinInvrement;
        else
            _state = NotEqualWithinIncrement;
    }
    //update remembered value
    if (EqualWithinInvrement != _state)
        _lastInformedValue = dataValue;
}

template <class T>
void CovIncrementHandler<T>::visit(Real &data)
{
    T dataValue = data.value();
    comparison_helper(dataValue);
}

template <class T>
void CovIncrementHandler<T>::visit(Double &data)
{
    T dataValue = data.value();
    comparison_helper(dataValue);
}

template <class T>
void CovIncrementHandler<T>::visit(UnsignedInteger &data)
{
    T dataValue = data.value();
    comparison_helper(dataValue);
}

template <class T>
void CovIncrementHandler<T>::visit(SignedInteger &data)
{
    T dataValue = data.value();
    comparison_helper(dataValue);
}

template <class T>
void CovIncrementHandler<T>::visit(BacnetDataInterface &data)
{
    Q_UNUSED(data);
    qDebug("CovIncrementHandler<T>::visit(BacnetDataInterface&) always marks as not equal!");
    //! \todo Or maybe we should compare qvariants?
    _state = NotEqualWithinIncrement;
}

namespace Bacnet {//otherwise compilator complains there are specializations in a differenet namespace

template <>
void CovIncrementHandler<float>::visit(BacnetDataInterface &data)
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

template <>
void CovIncrementHandler<double>::visit(BacnetDataInterface &data)
{
    QVariant dataValue = data.toInternal();
    if (dataValue.canConvert(QVariant::Double)) {
        bool ok;
        double value = dataValue.toDouble(&ok);
        Q_ASSERT(ok);
        if (ok)
            comparison_helper(value);
    }
}

template <>
void CovIncrementHandler<qint32>::visit(BacnetDataInterface &data)
{
    QVariant dataValue = data.toInternal();
    if (dataValue.canConvert(QVariant::Int)) {
        bool ok;
        qint32 value = dataValue.toInt(&ok);
        Q_ASSERT(ok);
        if (ok)
            comparison_helper(value);
    }
}

template <>
void CovIncrementHandler<quint32>::visit(BacnetDataInterface &data)
{
    QVariant dataValue = data.toInternal();
    if (dataValue.canConvert(QVariant::Int)) {
        bool ok;
        quint32 value = dataValue.toInt(&ok);
        Q_ASSERT(ok);
        if (ok)
            comparison_helper(value);
    }
}

}

//avoiding linking problems
template class CovIncrementHandler<float>;
template class CovIncrementHandler<double>;
template class CovIncrementHandler<quint32>;
template class CovIncrementHandler<qint32>;
