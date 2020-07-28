#pragma once
#ifndef SFLLOADINGDLG_H
#define SFLLOADINGDLG_H

#include "SFLDialogBase.h"

class QLabel;

class SFLLoadingDlg : public SFLDialogBase
{
    Q_OBJECT

public:

    explicit SFLLoadingDlg(QWidget *parent = 0);
    ~SFLLoadingDlg();

public:

    void SetText(
        const QString& text
    );

private:

    QLabel* m_text_label;

};

#endif // SFLLOADINGDLG_H
