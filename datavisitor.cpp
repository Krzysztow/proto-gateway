#include "datavisitor.h"

#include "bacnetprimitivedata.h"

using namespace Bacnet;

DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(Real, "f")
DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(Double, "f")
DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(UnsignedInteger, "d")
DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(SignedInteger, "d")

void DataVisitor::visit(BacnetArray &data) {
    qDebug("%s: array count is %d", __PRETTY_FUNCTION__, data.count());
}

//handling default cases
DEFINE_VISITOR_CASE_FOR_VISITABLE_INTERNAL(BacnetDataInterface)
