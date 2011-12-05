#ifndef SNGFACTORY_H
#define SNGFACTORY_H

#include <QDomElement>

namespace Sng {

class SngHandler;

class SngFactory
{
public:
    static SngHandler *createModule(QDomElement &sngConfig);
};

}

#endif // SNGFACTORY_H
