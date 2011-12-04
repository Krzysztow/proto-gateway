#include "configuratorhelper.h"

#include <QString>
#include <QStringBuilder>

using namespace ConfiguratorHelper;

QString ConfiguratorHelper::elementString(QDomElement &el) {
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

void ConfiguratorHelper::elementError(QDomElement &el, const char* attrWithError, const char *addInfo) {
    QString errorElement = elementString(el);
    qDebug("Error occured while parsing attribute *%s* of element \n\t%s.\n%s\n", attrWithError, qPrintable(errorElement), addInfo);
}

QBitArray ConfiguratorHelper::bitArrayFromString(QString &array, bool *ok)
{
    return bitArrayFromChar(array.toLatin1().data(), array.size(), ok);
}

QBitArray ConfiguratorHelper::bitArrayFromChar(const char *data, int size, bool *ok)
{
    if (0 != ok)
        *ok = true;

    if (strncmp(data, "0x", 2) == 0 ||
            strncmp(data, "0X", 2) == 0) {//it's hexadecimal encoded
        const int significantDataSize = size - 2;
        QBitArray result(8/2 * significantDataSize, false);
        int processedBitIdx =  result.size() - 1;
        quint8 mask;
        const char *parsedNibble = &data[1];

        while(processedBitIdx >= 0) {
            ++parsedNibble;
            char nibble = *parsedNibble;
            if ( ('0' <= nibble) && (nibble <= '9') ) {
                nibble = nibble - '0';
            } else if ( ('A' <= nibble) && (nibble <= 'F') ) {
                nibble = 10 + nibble - 'A';
            } else if ( ('a' <= nibble) && (nibble <= 'f') ) {
                nibble = 10 + nibble - 'a';
            } else {
                if (0 != ok) *ok = false;
                break;
            }

            mask = 0x08;
            for (uint i = 0; i < 8/2 * sizeof(quint8); ++i) {
                if (mask & nibble)
                    result.setBit(processedBitIdx, true);
                mask >>= 1;
                --processedBitIdx;
            }
        }

        return result;
    } else if ( ((strncmp(data, "B'", 2) == 0) || (strncmp(data, "b'", 2) == 0)) &&
                data[size - 1] == '\'') {
        const int significantDataSize = size - 3;
        QBitArray result(significantDataSize, false);
        const char *bitPtr = &data[2];
        for (int i = significantDataSize - 1; i >= 0; --i) {
            if (*bitPtr == '1')
                result.setBit(i, true);
            ++bitPtr;
        }
        return result;
    } else
        if (0 != ok) *ok = false;

    return QBitArray();
}

