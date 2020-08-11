#include "SFLTunnelWidget.h"

#include <QCheckBox>
#include <QBoxLayout>
#include <QPushButton>

#include "SFLTunnelTableWidget.h"
#include "SFLMsgBox.h"

SFLTunnelWidget::SFLTunnelWidget(QWidget *parent)
    : QWidget(parent),
    m_tunnel_table_widget(nullptr)
{
    QWidget* tool_widget = new QWidget(this);

    m_select_all_check_box = new QCheckBox(tool_widget);
    m_select_all_check_box->setText(QString::fromLocal8Bit("全选"));
    connect(m_select_all_check_box, &QCheckBox::stateChanged, this, &SFLTunnelWidget::OnSelectAllCheckBoxStateChanged);

    QPushButton* start_selected_btn = new QPushButton(tool_widget);
    start_selected_btn->setText(QString::fromLocal8Bit("启动所选隧道"));
    connect(start_selected_btn, &QPushButton::clicked, this, &SFLTunnelWidget::OnStartSelectedBtnClicked);

    QPushButton* stop_selected_btn = new QPushButton(tool_widget);
    stop_selected_btn->setText(QString::fromLocal8Bit("停止所选隧道"));
    connect(stop_selected_btn, &QPushButton::clicked, this, &SFLTunnelWidget::OnStopSelectedBtnClicked);

    QPushButton* delete_selected_btn = new QPushButton(tool_widget);
    delete_selected_btn->setText(QString::fromLocal8Bit("删除所选隧道"));
    connect(delete_selected_btn, &QPushButton::clicked, this, &SFLTunnelWidget::OnDeleteSelectedBtnClicked);

    QHBoxLayout* tool_widget_h_layout = new QHBoxLayout(tool_widget);
    tool_widget_h_layout->setMargin(0);
    tool_widget_h_layout->addWidget(m_select_all_check_box);
    tool_widget_h_layout->addWidget(start_selected_btn);
    tool_widget_h_layout->addWidget(stop_selected_btn);
    tool_widget_h_layout->addWidget(delete_selected_btn);
    tool_widget_h_layout->addStretch();
    tool_widget->setLayout(tool_widget_h_layout);

    m_tunnel_table_widget = new SFLTunnelTableWidget(this);

    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addWidget(tool_widget);
    v_layout->addWidget(m_tunnel_table_widget);
    this->setLayout(v_layout);
}

SFLTunnelWidget::~SFLTunnelWidget()
{
    
}

void SFLTunnelWidget::InitTunnelWidget(
    const NodeItemInfo& node_item_info
) {
    m_tunnel_table_widget->InitTunnelTableWidget(node_item_info);
}

void SFLTunnelWidget::GetNodeItemInfo(
    NodeItemInfo& node_item_info
) {
    m_tunnel_table_widget->GetNodeItemInfo(node_item_info);
}

void SFLTunnelWidget::TerminateAllProcess(
) {
    m_tunnel_table_widget->TerminateAllProcess();
}

void SFLTunnelWidget::OnSelectAllCheckBoxStateChanged(
    int state
) {
    m_tunnel_table_widget->SetAllCheckBoxState(state);
}

void SFLTunnelWidget::OnStartSelectedBtnClicked(
) {
    m_tunnel_table_widget->StartStopSelectedTunnel(true);
}

void SFLTunnelWidget::OnStopSelectedBtnClicked(
) {
    m_tunnel_table_widget->StartStopSelectedTunnel(false);
}

void SFLTunnelWidget::OnDeleteSelectedBtnClicked(
) {
    SFLMsgBox::GetInstance()->SetBoxType(e_warning_type_yes_no);
    SFLMsgBox::GetInstance()->setText(QString::fromLocal8Bit("确定删除所选隧道吗"));
    if (SFLMsgBox::GetInstance()->exec() == QMessageBox::No) {
        return;
    }
    m_tunnel_table_widget->DeleteSelectedTunnel();
}