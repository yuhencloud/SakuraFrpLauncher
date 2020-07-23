#pragma once
#ifndef SAKURAFRPLAUNCHER_H
#define SAKURAFRPLAUNCHER_H

#include "SFLDialogBase.h"
#include <QEvent>
#include <QSystemTrayIcon>

#include "SFLCommonDefs.h"

class QSystemTrayIcon;
class QLineEdit;
class QCheckBox;
class SFLGroupTabWidget;

class SakuraFrpLauncher : public SFLDialogBase
{
    Q_OBJECT

public:

    SakuraFrpLauncher(QWidget *parent = 0);
    ~SakuraFrpLauncher();

public:

    void ShowTrayMessage(
        const TunnelProcess& tunnel_process
    );

protected:

    virtual void changeEvent(
        QEvent* e
    );

    virtual void closeEvent(
        QCloseEvent* e
    );

private:

    void InitTrayIcon(
    );

    QWidget* InitLoginWidget(
    );

    void InitTunnelsGroup(
        QVector<NodeItemInfo> node_item_info_list,
        QVector<TunnelItemInfo> tunnel_item_info_list
    );

private slots:

    void OnActivated(
        QSystemTrayIcon::ActivationReason reason
    );

    void OnOpenAction(
    );

    void OnCloseAction(
    );

    void OnLoginBtnClicked(
    );

private:

    QSystemTrayIcon* m_system_tray_icon;
    QLineEdit* m_line_edit;
    QCheckBox* m_tray_message_check_box;
    SFLGroupTabWidget* m_group_tab_widget;

};

#endif // SAKURAFRPLAUNCHER_H
