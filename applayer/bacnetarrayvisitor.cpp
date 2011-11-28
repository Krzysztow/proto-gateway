#include "bacnetarrayvisitor.h"

using namespace Bacnet;
BacnetArrayVisitor::BacnetArrayVisitor(quint32 idxToGet):
    _arrayIdx(idxToGet)
{
}

void BacnetArrayVisitor::visit(BacnetDataInterface &data)
{
    Q_UNUSED(data);
    Q_ASSERT(false);
}

void BacnetArrayVisitor::visit(Double &data)
{
    Q_UNUSED(data);
    Q_ASSERT(false);
}


void BacnetArrayVisitor::visit(Real &data)
{
    Q_UNUSED(data);
    Q_ASSERT(false);
}

void BacnetArrayVisitor::visit(SignedInteger &data)
{
    Q_UNUSED(data);
    Q_ASSERT(false);
}

void BacnetArrayVisitor::visit(UnsignedInteger &data)
{
    Q_UNUSED(data);
    Q_ASSERT(false);
}

void BacnetArrayVisitor::visit(BacnetArray &data)
{
    if (0 == _arrayIdx) {
        _dataGotten = BacnetDataInterfaceShared(new UnsignedInteger(data.count()));
    } else if (0 != ArrayIndexNotPresent) {
        if (data.count() >= _arrayIdx)
            _dataGotten = data.value().at(_arrayIdx - 1);
    }
}

BacnetDataInterfaceShared &BacnetArrayVisitor::dataGotten()
{
    return _dataGotten;
}
