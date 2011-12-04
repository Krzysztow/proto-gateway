#ifndef CONFIGURATORHELPER_H
#define CONFIGURATORHELPER_H

#include <QDomElement>
#include <QBitArray>

namespace ConfiguratorHelper {

QString elementString(QDomElement &el);
void elementError(QDomElement &el, const char* attrWithError, const char *addInfo = "");

QBitArray bitArrayFromString(QString &array, bool *ok = 0);
QBitArray bitArrayFromChar(const char *data, int size, bool *ok = 0);

}
#endif // CONFIGURATORHELPER_H
