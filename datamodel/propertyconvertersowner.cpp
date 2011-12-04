#include "propertyconvertersowner.h"

#include "propertyconverter.h"

using namespace DataModelNS;

PropertyConvertersOwner *PropertyConvertersOwner::_instance = 0;

PropertyConvertersOwner::PropertyConvertersOwner():
    _universalConverter(new PropertyUniversalConverter())
{
}


PropertyConvertersOwner *PropertyConvertersOwner::instance()
{
    if (0 == _instance)
        _instance = new PropertyConvertersOwner();

    return _instance;
}

PropertyUniversalConverter *PropertyConvertersOwner::universalConverter()
{
    return _universalConverter;
}
