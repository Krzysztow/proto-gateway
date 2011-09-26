#include "covsupport.h"

using namespace Bacnet;

bool AnyCovSupport::hasChangeOccured(QVariant &actualValue)
{
    Q_UNUSED(actualValue);
    return true;//for other types than Real and Bool, whenever the value changes anyhow, we consider it as a significant change.
}

void AnyCovSupport::setCovIncrement(BacnetDataInterface *newIncrement)
{
    Q_CHECK_PTR(newIncrement);
    Q_ASSERT(false);
}

BacnetDataInterface *AnyCovSupport::covIncrement()
{
    return 0;
}

///////////////////////////////////////////////////////
/////////////////////RealCovSupport////////////////////
///////////////////////////////////////////////////////

//RealCovSupport::RealCovSupport(float covIncrement, float lastValue):
//        _lastValue(lastValue)
//{
//    QVariant value(covIncrement);
//    _covIncrement.setInternal(value);
//}

//void RealCovSupport::setCovIncrement(float covIncrement)
//{
//    QVariant value(covIncrement);
//    _covIncrement.setInternal(value);
//}

//bool RealCovSupport::hasChangeOccured(QVariant &actualValue)
//{
//    bool ok;
//    float covIncrement = _covIncrement.toInternal().toFloat(&ok);
//    Q_ASSERT(ok);
//    if (!ok) return true; //if something wrong happens during conversion, treat it that value has changed!
//    float actValue = actualValue.toFloat(&ok);
//    Q_ASSERT(ok);
//    if (!ok) return true;//same as above
//    actValue -= _lastValue;
//    return qAbs(actValue) >= covIncrement;
//}

//void RealCovSupport::setCovIncrement(BacnetDataInterface *newIncrement)
//{
//    Q_ASSERT(newIncrement);
//    QVariant value = newIncrement->toInternal();
//    _covIncrement.setInternal(value);
//}

//BacnetDataInterface *RealCovSupport::covIncrement()
//{
//    return &_covIncrement;
//}

///////////////////////////////////////////////////////
/////////////////////DoubleCovSupport//////////////////
///////////////////////////////////////////////////////

//DoubleCovSupport::DoubleCovSupport(double covIncrement, double lastValue):
//        _lastValue(lastValue)
//{
//    QVariant value(covIncrement);
//    _covIncrement.setInternal(value);
//}

//void DoubleCovSupport::setCovIncrement(double covIncrement)
//{
//    QVariant value(covIncrement);
//    _covIncrement.setInternal(value);
//}

//bool DoubleCovSupport::hasChangeOccured(QVariant &actualValue)
//{
//    bool ok;
//    double covIncrement = _covIncrement.toInternal().toDouble(&ok);
//    Q_ASSERT(ok);
//    if (!ok) return true; //if something wrong happens during conversion, treat it that value has changed!
//    double actValue = actualValue.toDouble(&ok);
//    Q_ASSERT(ok);
//    if (!ok) return true;//same as above
//    actValue -= _lastValue;
//    return qAbs(actValue) >= covIncrement;
//}

//void DoubleCovSupport::setCovIncrement(BacnetDataInterface *newIncrement)
//{
//    Q_ASSERT(newIncrement);
//    QVariant value = newIncrement->toInternal();
//    _covIncrement.setInternal(value);
//}

//BacnetDataInterface *DoubleCovSupport::covIncrement()
//{
//    return &_covIncrement;
//}


///////////////////////////////////////////////////////////
template <typename T, class T2>
ConcreteCovSupport<T, T2>::ConcreteCovSupport(T covIncrement, T lastValue):
        _lastValue(lastValue)
{
    QVariant value(covIncrement);
    _covIncrement.setInternal(value);
}

template <typename T, class T2>
void ConcreteCovSupport<T, T2>::setCovIncrement(T covIncrement)
{
    QVariant value(covIncrement);
    _covIncrement.setInternal(value);
}

template <typename T, class T2>
bool ConcreteCovSupport<T, T2>::hasChangeOccured(QVariant &actualValue)
{
    bool ok;
    T covIncrement = toT<T>(_covIncrement.toInternal(), &ok);
    Q_ASSERT(ok);
    if (!ok) return true; //if something wrong happens during conversion, treat it that value has changed!
    T actValue = toT<T>(actualValue, &ok);
    Q_ASSERT(ok);
    if (!ok) return true;//same as above
    actValue -= _lastValue;
    return qAbs(actValue) >= covIncrement;
}

template <typename T, class T2>
void ConcreteCovSupport<T, T2>::setCovIncrement(BacnetDataInterface *newIncrement)
{
    Q_ASSERT(newIncrement);
    QVariant value = newIncrement->toInternal();
    _covIncrement.setInternal(value);
}

template <typename T, class T2>
BacnetDataInterface *ConcreteCovSupport<T, T2>::covIncrement()
{
    return &_covIncrement;
}

//define those that will be used.
template class ConcreteCovSupport<float, Bacnet::Real>;
template class ConcreteCovSupport<double, Bacnet::Double>;
