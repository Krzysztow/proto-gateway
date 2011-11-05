#ifndef DATAVISITOR_H
#define DATAVISITOR_H

/**
  \todo Separate BacnetDataInterface from visitee interface.

  Whenever you want to use a BacnetDataInterface descendant with DataVisitor class to do something specific, you have to:
  - make sure, the visitor interface accepts your type, that means:
    * there has to be separate visit() function in DataVisitor for your class type. If there is not, declare it and give a default
    implemententation using macros:
    DECLARE_VISITOR_CASE_FOR_VISITABLE and DEFINE_VISITOR_CASE_FOR_VISITABLE*.
  - in your class implementation, add visit function declaration and definition by using DECLARE_VISITABLE_FUNCTION &
    DEFINE_VISITABLE_FUNCTION, respectively.
  */

//! Macro for declaring visitable interface function.
#define DECLARE_VISITABLE_FUNCTION(clazz) virtual void visit(DataVisitor *visitor);

//! Macro for defining visitable interface function - class instance dispatches itself to the visitor.
#define DEFINE_VISITABLE_FUNCTION(clazz) void clazz::visit(DataVisitor *visitor) {\
    Q_CHECK_PTR(visitor);       \
    if (0 != visitor)           \
    visitor->visit(*this);  \
    }

//! Macro for declaring visitor function for specific class.
#define DECLARE_VISITOR_CASE_FOR_VISITABLE(clazz) virtual void visit(clazz &data);

/**
    Macro defining default visitor function for specific class, with descriptive information it was invoked.
    specifier (within quotes) is the format specifier, as in printf() function.
*/
#define DEFINE_VISITOR_CASE_FOR_VISITABLE_DESCRIPTIVE(clazz, specifier) void DataVisitor::visit(clazz &data) {\
    qDebug("Function %s ivoked with %s and has value %"specifier, __PRETTY_FUNCTION__, #clazz, data.value());\
    }

/**
    Macro defining default visitor function for specific class, with descriptive information it was invoked and using
    toInternal() BacnetDataInterface function.
*/
#define DEFINE_VISITOR_CASE_FOR_VISITABLE_INTERNAL(clazz) void DataVisitor::visit(clazz &data) {\
    qDebug("Function %s ivoked with %s and has value %s", __PRETTY_FUNCTION__, #clazz, qPrintable(data.toInternal().toString()));\
    }

/**
    Macro defining default visitor empty function. This (and other) macros should be used when DataVisitor methods are not to be
    pure virtual, so that DataVisotor descendants may not implement all the methods.
*/
#define DEFINE_VISITOR_CASE_FOR_VISITABLE_INTERNAL_DEFAULT(clazz) void DataVisitor::visit(clazz &data){ Q_UNUSED(data); }

namespace Bacnet {

class Real;
class Double;
class UnsignedInteger;
class SignedInteger;
class BacnetDataInterface;

class DataVisitor
{
public://visitor interfaces
    DECLARE_VISITOR_CASE_FOR_VISITABLE(Real)
    DECLARE_VISITOR_CASE_FOR_VISITABLE(Double)
    DECLARE_VISITOR_CASE_FOR_VISITABLE(UnsignedInteger)
    DECLARE_VISITOR_CASE_FOR_VISITABLE(SignedInteger)

    //general for unhandled cases
    DECLARE_VISITOR_CASE_FOR_VISITABLE(BacnetDataInterface)
};

}

#endif // DATAVISITOR_H
