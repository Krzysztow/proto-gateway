#include "datavisitor.h"

#include "bacnetprimitivedata.h"

using namespace Bacnet;

DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(Real, "f")
DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(Double, "f")
DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(UnsignedInteger, "d")
DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(SignedInteger, "d")

//handling default cases
DEFINE_VISITOR_CASE_FOR_VISITABLE_INTERNAL(BacnetDataInterface)
