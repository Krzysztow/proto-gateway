#ifndef DATAMODEL_PROPERTYCONVERTER_H
#define DATAMODEL_PROPERTYCONVERTER_H

#include <QVariant>

namespace DataModelNS {

class PropertyConverter
{
public:
    virtual bool convertFromInternal(QVariant &internalValue, QVariant &externalValue) = 0;
    virtual bool convertToInternal(QVariant &internalValue, QVariant &externalValue) = 0;
};


/** Class allows all the possible conversions
  */
class PropertyUniversalConverter:
        public PropertyConverter
{
public:
    virtual bool convertFromInternal(QVariant &internalValue, QVariant &externalValue) ;
    virtual bool convertToInternal(QVariant &internalValue, QVariant &externalValue) ;

protected:
    bool convertFromOneToTwohelper(QVariant &one, QVariant &two);
};

/** Property scales convertedr
  */
class PropertyScalerConverter:
        public PropertyConverter
{
public:
    PropertyScalerConverter(float multiplyFactorFromInternalToExternal);

public:
    virtual bool convertFromInternal(QVariant &internalValue, QVariant &externalValue) ;
    virtual bool convertToInternal(QVariant &internalValue, QVariant &externalValue) ;

    static QList<QVariant::Type> allowableTypes();

public:
    float factor() {return _multiplyFromInternalToExternal;}

private:
    bool fromOneToTwo_helper(QVariant &one, QVariant &two, bool doMultiply);

private:
    float _multiplyFromInternalToExternal;
};



} // namespace DataModel

#endif // DATAMODEL_PROPERTYCONVERTER_H
