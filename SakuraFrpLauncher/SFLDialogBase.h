#pragma once
#ifndef SFLDIALOGBASE_H
#define SFLDIALOGBASE_H

#include <QDialog>
#include <qevent.h>

class SFLDialogBase : public QDialog
{
    Q_OBJECT

public:

    SFLDialogBase(QWidget *parent = 0);
    ~SFLDialogBase();

protected:

    void keyPressEvent(
        QKeyEvent* e
    );

};

#endif // SFLDIALOGBASE_H
