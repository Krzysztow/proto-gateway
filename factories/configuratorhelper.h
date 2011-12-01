#ifndef CONFIGURATORHELPER_H
#define CONFIGURATORHELPER_H

#include <QDomElement>

QString elementString(QDomElement &el);
void elementError(QDomElement &el, const char* attrWithError, const char *addInfo = "");

#endif // CONFIGURATORHELPER_H
