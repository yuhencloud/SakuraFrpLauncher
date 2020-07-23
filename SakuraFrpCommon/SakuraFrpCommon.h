#ifndef SAKURAFRPCOMMON_H
#define SAKURAFRPCOMMON_H

#include "sakurafrpcommon_global.h"

#include <QString>

class SAKURAFRPCOMMON_EXPORT SakuraFrpCommon
{
public:

    SakuraFrpCommon();
    ~SakuraFrpCommon();

public:

    static void UnicodeTransfrom(
        QString& str
    );

    static QString NextUuid(
    );

private:

};

#endif // SAKURAFRPCOMMON_H
