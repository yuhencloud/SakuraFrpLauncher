#include "SFLCreateTunnelDlg.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

#include "SFLCommonDefs.h"
#include "SFLGlobalMgr.h"
#include "SFLDBMgr.h"
#include "SFLNetworkMgr.h"
#include "SFLMsgBox.h"
#include "SFLJsonHelper.h"

SFLCreateTunnelDlg::SFLCreateTunnelDlg(QWidget *parent) :
    SFLDialogBase(parent)
{
    this->setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setModal(true);
    this->setFixedSize(270, 220);
    this->setWindowTitle(QString::fromLocal8Bit("创建隧道"));
    this->setObjectName("SFLCreateTunnelDlg");

    QWidget* local_widget = new QWidget(this);
    m_local_ip_line_edit = new QLineEdit(local_widget);
    m_local_ip_line_edit->setPlaceholderText(QString::fromLocal8Bit("本地IP"));
    QPushButton* local_machine_btn = new QPushButton(local_widget);
    local_machine_btn->setText(QString::fromLocal8Bit("本机"));
    connect(local_machine_btn, &QPushButton::clicked, this, &SFLCreateTunnelDlg::OnLocalMachineBtnClicked);
    m_local_port_line_edit = new QLineEdit(local_widget);
    m_local_port_line_edit->setPlaceholderText(QString::fromLocal8Bit("本地端口"));
    m_local_port_line_edit->setFixedWidth(60);
    QHBoxLayout* local_widget_h_layout = new QHBoxLayout(local_widget);
    local_widget_h_layout->setMargin(0);
    local_widget_h_layout->addWidget(m_local_ip_line_edit);
    local_widget_h_layout->addWidget(local_machine_btn);
    local_widget_h_layout->addWidget(m_local_port_line_edit);
    local_widget->setLayout(local_widget_h_layout);

    m_tunnel_name_line_edit = new QLineEdit(this);
    m_tunnel_name_line_edit->setPlaceholderText(QString::fromLocal8Bit("隧道名称（留空随机）"));

    QWidget* remote_widget = new QWidget(this);
    m_tunnel_type_combox = new QComboBox(remote_widget);
    m_tunnel_type_combox->addItem("tcp");
    m_tunnel_type_combox->addItem("udp");
    m_tunnel_type_combox->setCurrentIndex(0);
    QLabel* remote_port_label = new QLabel(remote_widget);
    remote_port_label->setText(QString::fromLocal8Bit("远程端口:"));
    remote_port_label->setToolTip(QString::fromLocal8Bit("0随机，范围10240~65535"));
    m_remote_port_line_edit = new QLineEdit(remote_widget);
    m_remote_port_line_edit->setPlaceholderText(QString::fromLocal8Bit("0随机，范围10240~65535"));
    m_remote_port_line_edit->setToolTip(QString::fromLocal8Bit("0随机，范围10240~65535"));
    m_remote_port_line_edit->setText("0");
    QHBoxLayout* remote_widget_h_layout = new QHBoxLayout(remote_widget);
    remote_widget_h_layout->setMargin(0);
    remote_widget_h_layout->addWidget(m_tunnel_type_combox);
    remote_widget_h_layout->addSpacing(20);
    remote_widget_h_layout->addWidget(remote_port_label);
    remote_widget_h_layout->addWidget(m_remote_port_line_edit);
    remote_widget->setLayout(remote_widget_h_layout);

    m_server_combox = new QComboBox(this);

    QWidget* setting_widget = new QWidget(this);
    m_compression_check_box = new QCheckBox(setting_widget);
    m_compression_check_box->setText(QString::fromLocal8Bit("压缩数据"));
    m_compression_check_box->setChecked(false);
    m_encryption_check_box = new QCheckBox(setting_widget);
    m_encryption_check_box->setText(QString::fromLocal8Bit("加密传输"));
    m_encryption_check_box->setChecked(false);
    QHBoxLayout* setting_widget_h_layout = new QHBoxLayout(setting_widget);
    setting_widget_h_layout->setMargin(0);
    setting_widget_h_layout->addWidget(m_compression_check_box);
    setting_widget_h_layout->addWidget(m_encryption_check_box);
    setting_widget->setLayout(setting_widget_h_layout);

    QPushButton* create_tunnel_btn = new QPushButton(this);
    create_tunnel_btn->setText(QString::fromLocal8Bit("创建隧道"));
    connect(create_tunnel_btn, &QPushButton::clicked, this, &SFLCreateTunnelDlg::OnCreateTunnelBtnClicked);

    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addWidget(local_widget);
    v_layout->addWidget(m_tunnel_name_line_edit);
    v_layout->addWidget(remote_widget);
    v_layout->addWidget(m_server_combox);
    v_layout->addWidget(setting_widget);
    v_layout->addWidget(create_tunnel_btn);
    this->setLayout(v_layout);
}

SFLCreateTunnelDlg::~SFLCreateTunnelDlg()
{
}

void SFLCreateTunnelDlg::InitCreateTunnelDlg(
) {
    // 清空
    m_local_ip_line_edit->setText("");
    m_local_port_line_edit->setText("");
    m_tunnel_name_line_edit->setText("");
    m_tunnel_type_combox->setCurrentIndex(0);
    m_remote_port_line_edit->setText("0");
    m_server_combox->clear();
    m_compression_check_box->setChecked(false);
    m_encryption_check_box->setChecked(false);
    // 初始化服务器列表
    NodeInfo node_info;
    SFLGlobalMgr::GetInstance()->GetNodeInfo(node_info);
    for (auto node : node_info.node_item_info_list) {
        if (node.node_accept_new) {
            QVariant data;
            data.setValue(node);
            m_server_combox->addItem("#" + QString::number(node.node_id) + " " + node.node_name, data);
        }
    }
}

void SFLCreateTunnelDlg::OnLocalMachineBtnClicked(
) {
    m_local_ip_line_edit->setText("127.0.0.1");
}

void SFLCreateTunnelDlg::OnCreateTunnelBtnClicked(
) {
    QString local_ip = m_local_ip_line_edit->text();
    if (local_ip.split(".").size() != 4) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("IP地址格式错误"));
        SFLMsgBox::GetInstance()->show();
        return;
    }
    QString local_port = m_local_port_line_edit->text();
    if (local_port.toInt() <= 0 || local_port.toInt() > 65535) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("本地端口超过设置范围，请设置0~65535"));
        SFLMsgBox::GetInstance()->show();
        return;
    }
    QString tunnel_name = m_tunnel_name_line_edit->text();
    QString tunnel_type = m_tunnel_type_combox->currentText();
    QString remote_port = m_remote_port_line_edit->text();
    if (remote_port.toInt() != 0 && (remote_port.toInt() <= 10240 || remote_port.toInt() > 65535)) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("远程端口超过设置范围，10240~65535"));
        SFLMsgBox::GetInstance()->show();
        return;
    }
    QVariant data = m_server_combox->currentData();
    NodeItemInfo node_item_info = data.value<NodeItemInfo>();
    QString server_id = QString::number(node_item_info.node_id);

    QString compression = "false";
    if (m_compression_check_box->isChecked()) {
        compression = "true";
    } else {
        compression = "false";
    }

    QString encryption = "false";
    if (m_encryption_check_box->isChecked()) {
        encryption = "true";
    } else {
        encryption = "false";
    }

    SFLLoadingDlg* login_dlg = SFLGlobalMgr::GetInstance()->LoadingDlg();
    login_dlg->SetText(QString::fromLocal8Bit("正在创建隧道..."));
    login_dlg->show();

    QString create_tunnel_url = sakura_frp_domain + uri_create_tunnel;
    QString token = "";
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_token, token);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
    create_tunnel_url += 
        "token=" + token
        + "&type=" + tunnel_type
        + "&name=" + tunnel_name
        + "&node=" + server_id
        + "&local_ip=" + local_ip
        + "&local_port=" + local_port
        + "&encryption=" + encryption
        + "&compression=" + compression
        + "&remote_port=" + remote_port;
    QString create_tunnel_json = "";
    SFLNetworkMgr().GetData(create_tunnel_url, create_tunnel_json, 60000);
    login_dlg->hide();

    CreateTunnelInfo create_tunnel_info;
    bool create_tunnel_suc = SFLJsonHelper().ParseCreateTunnelStringToStruct(create_tunnel_json, create_tunnel_info);
    if (!create_tunnel_suc) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("网络或服务器错误，请稍后重试"));
        SFLMsgBox::GetInstance()->show();
        return;
    }
    if (!create_tunnel_info.success) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText(create_tunnel_info.message);
        SFLMsgBox::GetInstance()->show();
        return;
    }

    this->hide();

    // 创建成功
    SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
    QString text = QString::fromLocal8Bit("创建成功");
    text += "\t" + QString::fromLocal8Bit("隧道ID:") + QString::number(create_tunnel_info.tunnel_item_info.tunnel_id)
        + "\t" + QString::fromLocal8Bit("隧道名称:") + create_tunnel_info.tunnel_item_info.tunnel_name
        + "\t" + QString::fromLocal8Bit("隧道类型:") + create_tunnel_info.tunnel_item_info.tunnel_type
        + "\t" + QString::fromLocal8Bit("节点ID:") + QString::number(create_tunnel_info.tunnel_item_info.node_item_info.node_id)
        + "\t" + QString::fromLocal8Bit("隧道描述:") + create_tunnel_info.tunnel_item_info.tunnel_description;
    SFLMsgBox::GetInstance()->setText(text);
    SFLMsgBox::GetInstance()->exec();

    // 刷新列表
    SFLGlobalMgr::GetInstance()->Launcher()->OnGetTunnelBtnClicked();
}