#ifndef BACNET_BACNETARRAYVISITOR_H
#define BACNET_BACNETARRAYVISITOR_H

#include "bacnetcommon.h"
#include "datavisitor.h"
#include "bacnetprimitivedata.h"

namespace Bacnet {

class BacnetArrayVisitor:
        public DataVisitor
{
public:
    BacnetArrayVisitor(quint32 idxToGet = ArrayIndexNotPresent);

public://overrridden DataVisitor methods
    virtual void visit(Real &data);
    virtual void visit(Double &data);
    virtual void visit(UnsignedInteger &data);
    virtual void visit(SignedInteger &data);
    virtual void visit(BacnetArray &data);
    //general for unhandled cases
    virtual void visit(BacnetDataInterface &data);

public:
    BacnetDataInterfaceShared &dataGotten();

private:
    BacnetDataInterfaceShared _dataGotten;
    quint32 _arrayIdx;
};

} // namespace Bacnet

#endif // BACNET_BACNETARRAYVISITOR_H
