#ifndef DATAMODELNS_PROPERTYCONVERTERS_H
#define DATAMODELNS_PROPERTYCONVERTERS_H

namespace DataModelNS {

class PropertyUniversalConverter;

class PropertyConvertersOwner
{
public:
    static PropertyConvertersOwner *instance();

    PropertyUniversalConverter *universalConverter();

private:
    PropertyConvertersOwner();
    static PropertyConvertersOwner *_instance;

    PropertyUniversalConverter *_universalConverter;
};

} // namespace DataModelNS

#endif // DATAMODELNS_PROPERTYCONVERTERS_H
