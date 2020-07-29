#include "SFLUtility.h"

#include <QUuid>

SFLUtility::SFLUtility()
{

}

SFLUtility::~SFLUtility()
{

}

void SFLUtility::UnicodeTransfrom(
    QString& str
) {
    do {
        int idx = str.indexOf("\\u");
        QString str_hex = str.mid(idx, 6);
        str_hex = str_hex.replace("\\u", QString());
        int n_hex = str_hex.toInt(0, 16);
        str.replace(idx, 6, QChar(n_hex));
    } while (str.indexOf("\\u") != -1);
}

QString SFLUtility::NextUuid(
) {
    return QUuid::createUuid().toString().remove("{").remove("}").remove("-");
}