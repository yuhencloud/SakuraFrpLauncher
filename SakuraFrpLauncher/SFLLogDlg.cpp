#include "SFLLogDlg.h"

#include <QBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QScrollBar>

SFLLogDlg::SFLLogDlg(QWidget *parent) :
    SFLDialogBase(parent),
    m_log_text_edit(nullptr)
{
    this->setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    this->setModal(false);
    this->setMinimumSize(1020, 400);
    this->setWindowTitle(QStringLiteral("查看日志"));

    m_info_label = new QLabel(this);
    m_info_label->setText("");

    m_log_text_edit = new QTextEdit(this);
    m_log_text_edit->setObjectName("SFLLogDlg_log_text_edit");
    m_log_text_edit->setTextColor(QColor(Qt::white));
    m_log_text_edit->setAcceptRichText(true);
    m_log_text_edit->setUndoRedoEnabled(false);
    m_log_text_edit->setReadOnly(true);
    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addWidget(m_info_label);
    v_layout->addWidget(m_log_text_edit);
    this->setLayout(v_layout);
}

SFLLogDlg::~SFLLogDlg()
{
}

void SFLLogDlg::UpdateLog(
    const TunnelProcess& tunnel_process
) {
    m_tunnel_process = tunnel_process;
    m_info_label->setText(QStringLiteral("隧道ID:") + QString::number(tunnel_process.tunnel_item_info.tunnel_id) + "\t" + QStringLiteral("隧道：") + tunnel_process.tunnel_item_info.tunnel_name);
    m_log_text_edit->setText(tunnel_process.log_text);
    m_log_text_edit->verticalScrollBar()->setValue(m_log_text_edit->verticalScrollBar()->maximum());
}

int SFLLogDlg::TunnelID(
) {
    return m_tunnel_process.tunnel_item_info.tunnel_id;
}