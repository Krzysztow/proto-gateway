#include "configuratorhelper.h"

#include <QString>
#include <QStringBuilder>

QString elementString(QDomElement &el) {
    QString str;
    str += "<"%el.tagName()%" ";

    QDomNamedNodeMap attrs = el.attributes();
    int count = attrs.count();
    for (int i = 0; i < count; ++i) {
        str += attrs.item(i).toAttr().name()%"=\""%attrs.item(i).toAttr().value()%"\" ";
    }
    str += "/>";

    return str;
}

void elementError(QDomElement &el, const char* attrWithError, const char *addInfo) {
    QString errorElement = elementString(el);
    qDebug("Error occured while parsing attribute *%s* of element \n\t%s.\n%s\n", attrWithError, qPrintable(errorElement), addInfo);
}


