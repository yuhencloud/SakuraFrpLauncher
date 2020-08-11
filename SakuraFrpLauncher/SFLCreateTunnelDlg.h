#pragma once
#ifndef SFLCREATETUNNELDLG_H
#define SFLCREATETUNNELDLG_H

#include "SFLDialogBase.h"

class QLineEdit;
class QComboBox;
class QCheckBox;

class SFLCreateTunnelDlg : public SFLDialogBase
{
    Q_OBJECT

public:

    explicit SFLCreateTunnelDlg(QWidget *parent = 0);
    ~SFLCreateTunnelDlg();

public:

    void InitCreateTunnelDlg(
    );

private slots:

    void OnLocalMachineBtnClicked(
    );

    void OnCreateTunnelBtnClicked(
    );

private:

    QLineEdit* m_local_ip_line_edit;
    QLineEdit* m_local_port_line_edit;
    QLineEdit* m_tunnel_name_line_edit;
    QComboBox* m_tunnel_type_combox;
    QLineEdit* m_remote_port_line_edit;
    QComboBox* m_server_combox;
    QCheckBox* m_compression_check_box;
    QCheckBox* m_encryption_check_box;

};

#endif // SFLCREATETUNNELDLG_H
