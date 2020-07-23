#pragma once
#ifndef SFLLOGDLG_H
#define SFLLOGDLG_H

#include "SFLDialogBase.h"
#include "SFLCommonDefs.h"

class QLabel;
class QTextEdit;

class SFLLogDlg : public SFLDialogBase
{
    Q_OBJECT

public:

    explicit SFLLogDlg(QWidget *parent = 0);
    ~SFLLogDlg();

public:

    void UpdateLog(
        const TunnelProcess& tunnel_process
    );

    int TunnelID(
    );

private:

    TunnelProcess m_tunnel_process;
    QLabel* m_info_label;
    QTextEdit* m_log_text_edit;

};

#endif // SFLLOGDLG_H
