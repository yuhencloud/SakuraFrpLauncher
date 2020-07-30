#include "SakuraFrpLauncher.h"

#include <QMenu>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>

#include "SFLNetworkMgr.h"
#include "SFLDBMgr.h"
#include "SFLLoadingDlg.h"
#include "SFLGlobalMgr.h"
#include "SFLJsonHelper.h"
#include "SFLGroupTabWidget.h"
#include "SFLMsgBox.h"

SakuraFrpLauncher::SakuraFrpLauncher(QWidget *parent)
    : SFLDialogBase(parent),
    m_system_tray_icon(nullptr),
    m_cipher_line_edit(nullptr),
    m_group_tab_widget(nullptr)
{
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setFixedSize(900, 400);
    this->setWindowIcon(QIcon(":/Resources/images/icon.ico"));
    QString version = "";
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_version, version);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    this->setWindowTitle(QString::fromLocal8Bit("SakuraFrpLauncher Powered By 雨痕云") + version);

    // 初始化登录
    QWidget* login_widget = InitLoginWidget();
    m_group_tab_widget = new SFLGroupTabWidget(this);

    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addWidget(login_widget);
    v_layout->addWidget(m_group_tab_widget);
    this->setLayout(v_layout);

    // 初始化托盘图标
    InitTrayIcon();

    // 初始化加载框
    SFLGlobalMgr::GetInstance()->SetLoadingDlg(new SFLLoadingDlg());

    SFLGlobalMgr::GetInstance()->SetLauncher(this);

    m_auto_get_tunnel_timer = new QTimer(this);
    connect(m_auto_get_tunnel_timer, SIGNAL(timeout()), this, SLOT(OnAutoGetTunnerTimeout()));

    QString auto_get_tunnel = "0";
    db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_auto_get_tunnel, auto_get_tunnel);
    if ("1" == auto_get_tunnel) {
        // 自动登录
        InitTabWidget();

        // 启动timer
        QString auto_get_tunnel_time = "";
        SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_auto_get_tunnel_time, auto_get_tunnel_time);
        m_auto_get_tunnel_timer->start(auto_get_tunnel_time.toInt());
    }
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
}

SakuraFrpLauncher::~SakuraFrpLauncher()
{

}

void SakuraFrpLauncher::InitTrayIcon(
) {
    // 创建系统托盘图标
    QMenu* tray_menu = new QMenu(this);
    QAction* open_action = new QAction(this);
    open_action->setText(QString::fromLocal8Bit("打开"));
    connect(open_action, &QAction::triggered, this, &SakuraFrpLauncher::OnOpenAction);
    QAction* quit_action = new QAction(this);
    quit_action->setText(QString::fromLocal8Bit("退出"));
    connect(quit_action, &QAction::triggered, this, &SakuraFrpLauncher::OnCloseAction);
    tray_menu->addAction(open_action);
    tray_menu->addAction(quit_action);

    m_system_tray_icon = new QSystemTrayIcon(this);
    m_system_tray_icon->setToolTip(QString::fromLocal8Bit("Sakura Frp客户端"));
    m_system_tray_icon->setIcon(QIcon(":/Resources/images/icon.ico"));
    connect(m_system_tray_icon, &QSystemTrayIcon::activated, this, &SakuraFrpLauncher::OnActivated);
    connect(m_system_tray_icon, &QSystemTrayIcon::messageClicked, this, &SakuraFrpLauncher::showNormal);
    m_system_tray_icon->setContextMenu(tray_menu);
    m_system_tray_icon->show();
}

void SakuraFrpLauncher::OnActivated(
    QSystemTrayIcon::ActivationReason reason
) {
    switch (reason) {
        case QSystemTrayIcon::Unknown: {
            break;
        }
        case QSystemTrayIcon::Context: {
            break;
        }
        case QSystemTrayIcon::DoubleClick: {
            this->showNormal();
            break;
        }
        case QSystemTrayIcon::Trigger: {
            this->showNormal();
            break;
        }
        case QSystemTrayIcon::MiddleClick: {
            break;
        }
        default: {
            break;
        }
    }
}

void SakuraFrpLauncher::OnOpenAction(
) {
    this->showNormal();
}

void SakuraFrpLauncher::OnCloseAction(
) {
    this->close();
}

void SakuraFrpLauncher::changeEvent(
    QEvent* e
) {
    // 最小化到托盘
    if (this->windowState() == Qt::WindowMinimized) {
        this->hide();
    }
}

void SakuraFrpLauncher::closeEvent(
    QCloseEvent* e
) {
    SFLMsgBox::GetInstance()->SetBoxType(e_warning_type_yes_no);
    SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("确定要退出程序吗，退出后所有隧道都会关闭"));
    if (SFLMsgBox::GetInstance()->exec() == QMessageBox::Yes) {
        ;
    } else {
        e->ignore();
        return;
    }
    QDialog::closeEvent(e);
}

QWidget* SakuraFrpLauncher::InitLoginWidget(
) {
    QWidget* login_widget = new QWidget(this);
    QHBoxLayout* login_widget_h_layout = new QHBoxLayout(login_widget);
    login_widget_h_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_cipher_line_edit = new QLineEdit(login_widget);
    m_cipher_line_edit->setObjectName("SakuraFrpLauncher_cipher_line_edit");
    m_cipher_line_edit->setPlaceholderText(QString::fromLocal8Bit("请输入登录密钥"));
    m_cipher_line_edit->setEchoMode(QLineEdit::Password);

    QPushButton* login_btn = new QPushButton(login_widget);
    login_btn->setText(QString::fromLocal8Bit("获取隧道"));
    connect(login_btn, &QPushButton::clicked, this, &SakuraFrpLauncher::OnLoginBtnClicked);

    m_auto_get_tunnel_check_box = new QCheckBox(login_widget);
    m_auto_get_tunnel_check_box->setText(QString::fromLocal8Bit("自动获取隧道"));
    QString auto_get_tunnel = "0";
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_auto_get_tunnel, auto_get_tunnel);
    if ("1" == auto_get_tunnel) {
        m_auto_get_tunnel_check_box->setChecked(true);
    } else {
        m_auto_get_tunnel_check_box->setChecked(false);
    }
    connect(m_auto_get_tunnel_check_box, SIGNAL(stateChanged(int)), this, SLOT(OnAutoGetTunnelCheckBoxStateChanged(int)));

    m_auto_start_process_check_box = new QCheckBox(login_widget);
    m_auto_start_process_check_box->setText(QString::fromLocal8Bit("自动启动隧道"));
    QString auto_start_process = "0";
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_auto_start_process, auto_start_process);
    if ("1" == auto_start_process) {
        m_auto_start_process_check_box->setChecked(true);
    } else {
        m_auto_start_process_check_box->setChecked(false);
    }
    connect(m_auto_start_process_check_box, SIGNAL(stateChanged(int)), this, SLOT(OnAutoStartProcessCheckBoxStateChanged(int)));

    m_tray_message_check_box = new QCheckBox(login_widget);
    m_tray_message_check_box->setText(QString::fromLocal8Bit("托盘通知"));
    QString tray_message = "0";
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_tray_message, tray_message);
    if ("1" == tray_message) {
        m_tray_message_check_box->setChecked(true);
    } else {
        m_tray_message_check_box->setChecked(false);
    }
    connect(m_tray_message_check_box, SIGNAL(stateChanged(int)), this, SLOT(OnTrayMessageCheckBoxStateChanged(int)));

    login_widget_h_layout->addWidget(m_cipher_line_edit);
    login_widget_h_layout->addWidget(login_btn);
    login_widget_h_layout->addStretch();
    login_widget_h_layout->addWidget(m_auto_get_tunnel_check_box);
    login_widget_h_layout->addWidget(m_auto_start_process_check_box);
    login_widget_h_layout->addWidget(m_tray_message_check_box);

    login_widget->setLayout(login_widget_h_layout);

    // 读取存储的token
    QString token = "";
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_token, token);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    m_cipher_line_edit->setText(token);

    return login_widget;
}

void SakuraFrpLauncher::OnLoginBtnClicked(
) {
    SFLLoadingDlg* login_dlg = SFLGlobalMgr::GetInstance()->LoadingDlg();
    login_dlg->SetText(QString::fromLocal8Bit("正在获取隧道列表..."));
    login_dlg->show();
    InitTabWidget();
    login_dlg->hide();
}

void SakuraFrpLauncher::InitTabWidget(
) {
    QString token = m_cipher_line_edit->text();
    if (token.isEmpty()) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("密钥不能为空"));
        SFLMsgBox::GetInstance()->show();
        return;
    }

    QString node_url = sakura_frp_domain + uri_get_nodes;
    node_url += "token=" + token;
    QString node_retsult = "";
    SFLNetworkMgr().GetData(node_url, node_retsult, 20000);

    // 进行json解析
    NodeInfo node_info;
    bool node_suc = SFLJsonHelper().ParseNodesStringToStruct(node_retsult, node_info);
    if (!node_suc) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("网络或服务器错误，请稍后重试"));
        SFLMsgBox::GetInstance()->show();
        return;
    }
    if (!node_info.success) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(node_info.message);
        SFLMsgBox::GetInstance()->show();
        return;
    }
    SFLGlobalMgr::GetInstance()->SetNodeInfo(node_info);

    // token入库
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_token, token);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);

    // 获取隧道列表
    QString tunnel_url = sakura_frp_domain + uri_get_tunnels;
    tunnel_url += "token=" + token;
    QString tunnel_retsult = "";
    SFLNetworkMgr().GetData(tunnel_url, tunnel_retsult, 20000);

    // 禁用输入框，不再允许修改密钥
    m_cipher_line_edit->setDisabled(true);

    // 进行json解析
    TunnelInfo tunnel_info;
    bool tunnel_suc = SFLJsonHelper().ParseTunnelsStringToStruct(tunnel_retsult, tunnel_info);
    if (!tunnel_suc) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("网络或服务器错误，请稍后重试"));
        SFLMsgBox::GetInstance()->show();
        return;
    }
    if (!tunnel_info.success) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(tunnel_info.message);
        SFLMsgBox::GetInstance()->show();
        return;
    }
    SFLGlobalMgr::GetInstance()->SetTunnelInfo(tunnel_info);

    // 初始化隧道数据库
    InitTunnels(node_info.node_item_info_list, tunnel_info.tunnel_item_info_list);

    // 初始化隧道组tab
    m_group_tab_widget->InitTabWidget();
}

void SakuraFrpLauncher::InitTunnels(
    QVector<NodeItemInfo> node_item_info_list,
    QVector<TunnelItemInfo> tunnel_item_info_list
) {
    for (int i = 0; i < tunnel_item_info_list.size(); ++i) {
        tunnel_item_info_list[i].tunnel_index = i;
		for (auto node_item_info : node_item_info_list) {
			if (node_item_info.node_id == tunnel_item_info_list[i].node_item_info.node_id) {
				tunnel_item_info_list[i].node_item_info.node_name = node_item_info.node_name;
                tunnel_item_info_list[i].node_item_info.node_accept_new = node_item_info.node_accept_new;
			}
		}
    }
	QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->BeginTransaction(db);
    SFLDBMgr::GetInstance()->DeleteTunnel(db);
    SFLDBMgr::GetInstance()->InsertTunnel(db, tunnel_item_info_list);
    SFLDBMgr::GetInstance()->EndTransaction(db);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
}

void SakuraFrpLauncher::ShowTrayMessage(
    const TunnelProcess& tunnel_process
) {
    QString tray_message = "0";
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_tray_message, tray_message);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    if (tray_message != "1") {
        return;
    }

    QString title = "";
    QSystemTrayIcon::MessageIcon icon;
    if (e_running_state_warnning == tunnel_process.running_state) {
        title = QString::fromLocal8Bit("警告");
        icon = QSystemTrayIcon::Warning;
    } else if (e_running_state_error == tunnel_process.running_state) {
        title = QString::fromLocal8Bit("错误");
        icon = QSystemTrayIcon::Critical;
    } else {
        return;
    }

    QString message = "";
    message += QString::number(tunnel_process.tunnel_item_info.tunnel_id) + " " + tunnel_process.tunnel_item_info.tunnel_name + " " + title;
    m_system_tray_icon->showMessage(title, message, icon);
}

void SakuraFrpLauncher::OnAutoGetTunnelCheckBoxStateChanged(
    int checked
) {
    if (checked == Qt::Checked) {
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_auto_get_tunnel, "1");
        QString auto_get_tunnel_time = "";
        SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_auto_get_tunnel_time, auto_get_tunnel_time);
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
        m_auto_get_tunnel_timer->start(auto_get_tunnel_time.toInt());
    } else if (checked == Qt::Unchecked) {
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_auto_get_tunnel, "0");
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
        m_auto_get_tunnel_timer->stop();
    }
}

void SakuraFrpLauncher::OnAutoStartProcessCheckBoxStateChanged(
    int checked
) {
    if (checked == Qt::Checked) {
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_auto_start_process, "1");
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    } else if (checked == Qt::Unchecked) {
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_auto_start_process, "0");
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    }
}

void SakuraFrpLauncher::OnTrayMessageCheckBoxStateChanged(
    int checked
) {
    if (checked == Qt::Checked) {
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_tray_message, "1");
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    } else if (checked == Qt::Unchecked) {
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->UpdateValueByKey(db, sfl_tray_message, "0");
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    }
}

void SakuraFrpLauncher::OnAutoGetTunnerTimeout(
) {
    InitTabWidget();
}
