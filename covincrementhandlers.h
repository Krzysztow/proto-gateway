#ifndef COVINCREMENTHANDLERS_H
#define COVINCREMENTHANDLERS_H

#include <QtCore>

#include "datavisitor.h"

namespace Bacnet {

class BacnetTagParser;

template <class T, class V>
class CovIncrementHandler:
        public DataVisitor
{
public:
    enum ComparisonState {
        NotEqualWithinIncrement,
        EqualWithinInvrement,
        Equal
    };


    /**
      The following visitor interfaces are mean to compare the value stored inside this class (\sa _lastInformedValue) with
      the value of the visitee (data). _comparisonResult will be set to the appropriate state.

      \note In case of REAL data types, we take care over increments.
      \note When the visit is called and lastInformed value differs with data.value() by more than increment, the _lastInformedValue
      is changed to the data.value(). So it updates automatically.
      */
public://visitor interfaces
    virtual void visit(Real &data);
    virtual void visit(Double &data);
    virtual void visit(UnsignedInteger &data);
    virtual void visit(SignedInteger &data);
    //general for unhandled cases
    virtual void visit(BacnetDataInterface &data);

public:
    bool isEqual() {return Equal == _state;}
    bool isEqualWithinIncrement() {return (Equal == _state ||
                                           EqualWithinInvrement == _state);}
    ComparisonState state() {return _state;}

public://functions used to parse and write cov increment
    qint32 toRaw(quint8 *ptrStart, quint16 buffLength);
    qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
    qint32 fromRaw(BacnetTagParser &parser);
    qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum);

    BacnetDataInterface *createValue();

private:
    void comparison_helper(V dataValue);

private:
    ComparisonState _state;

public:
    CovIncrementHandler(T &covIncremenet);
    CovIncrementHandler();

    void setIncrementValue(V &covIncrement);
    const V &incrementValue();

public:
    V _lastInformedValue;
    T _covIncrement;
};

typedef class CovIncrementHandler<Real, float> CovRealIcnrementHandler;
}

#endif // COVINCREMENTHANDLERS_H
