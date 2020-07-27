#pragma once
#ifndef SFLUTILITY_H
#define SFLUTILITY_H

#include <QObject>
#include <QString>

class SFLUtility : public QObject
{

    Q_OBJECT

public:

    SFLUtility();
    ~SFLUtility();

public:

    static void UnicodeTransfrom(
        QString& str
    );

    static QString NextUuid(
    );
#ifdef Q_OS_WIN
    static bool Is64BitSystem(
    );
#endif

private:

};

#endif // SFLUTILITY_H
