#include "SFLTunnelTableWidget.h"

#include <QHeaderView>
#include <QSignalMapper>
#include <QBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QApplication>
#include <QProcess>
#include <QDateTime>
#include <QTimer>

#include "SFLDBMgr.h"
#include "SFLGlobalMgr.h"
#include "SFLLogDlg.h"

SFLTunnelTableWidget::SFLTunnelTableWidget(QWidget *parent)
    : QTableWidget(parent),
    m_timer(nullptr),
    m_start_stop_mapper(nullptr),
    m_check_log_mapper(nullptr),
    m_log_dlg(nullptr)
{
    this->setColumnCount(11);
    QStringList header_label_list;
    header_label_list
        << QStringLiteral("序号")
        << QStringLiteral("隧道ID")
        << QStringLiteral("隧道")
        << QStringLiteral("类型")
        << QStringLiteral("节点")
        << QStringLiteral("描述")
        << QStringLiteral("启动时间")
        << QStringLiteral("运行时间")
        << QStringLiteral("进程状态")
        << QStringLiteral("运行状态")
        << QStringLiteral("操作");
    this->setHorizontalHeaderLabels(header_label_list);

    QHeaderView* header_view = verticalHeader();
    header_view->setHidden(true);

    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setAlternatingRowColors(true);
    this->horizontalHeader()->setSectionsClickable(false);
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->setFocusPolicy(Qt::NoFocus);

    m_log_dlg = new SFLLogDlg();
    m_log_dlg->hide();

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(OnTimeout()));
    m_timer->start(10000);
}

SFLTunnelTableWidget::~SFLTunnelTableWidget()
{
    if (nullptr != m_log_dlg) {
        delete m_log_dlg;
        m_log_dlg = nullptr;
    }
}

void SFLTunnelTableWidget::InitTunnelTableWidget(
    const QString& group_id
) {
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    QVector<TunnelItemInfo> tunnel_item_info_list;
    SFLDBMgr::GetInstance()->GetTunnelInfoByGroupID(db, group_id, tunnel_item_info_list);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);

    // 创建连接mapping
    if (nullptr != m_start_stop_mapper) {
        disconnect(m_start_stop_mapper, SIGNAL(mapped(int)), this, SLOT(OnStartStopBtnClicked(int)));
        QSignalMapper* temp_mapper = m_start_stop_mapper;
        temp_mapper->deleteLater();
        m_start_stop_mapper = nullptr;
    }
    m_start_stop_mapper = new QSignalMapper();
    QMetaObject::Connection conn = connect(m_start_stop_mapper, SIGNAL(mapped(int)), this, SLOT(OnStartStopBtnClicked(int)));

    if (nullptr != m_check_log_mapper) {
        disconnect(m_check_log_mapper, SIGNAL(mapped(int)), this, SLOT(OnCheckLogBtnClicked(int)));
        QSignalMapper* temp_mapper = m_check_log_mapper;
        temp_mapper->deleteLater();
        m_check_log_mapper = nullptr;
    }
    m_check_log_mapper = new QSignalMapper();
    conn = connect(m_check_log_mapper, SIGNAL(mapped(int)), this, SLOT(OnCheckLogBtnClicked(int)));

    this->setRowCount(tunnel_item_info_list.size());
    // 初始化列表
    for (int i = 0; i < tunnel_item_info_list.size(); ++i) {
        int col_index = 0;
        TunnelItemInfo tunnel_item_info = tunnel_item_info_list[i];

        QTableWidgetItem* index_item = new QTableWidgetItem();
        QString index = QString::number(i + 1);
        index_item->setText(index);
        index_item->setToolTip(index);
        index_item->setTextAlignment(Qt::AlignCenter);
        index_item->setFlags(index_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, index_item);

        QTableWidgetItem* tunnel_id_item = new QTableWidgetItem();
        QString tunnel_id = QString::number(tunnel_item_info.tunnel_id);
        tunnel_id_item->setText(tunnel_id);
        tunnel_id_item->setToolTip(tunnel_id);
        tunnel_id_item->setTextAlignment(Qt::AlignCenter);
        tunnel_id_item->setFlags(tunnel_id_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, tunnel_id_item);

        QTableWidgetItem* name_item = new QTableWidgetItem();
        name_item->setText(tunnel_item_info.name);
        name_item->setToolTip(tunnel_item_info.name);
        name_item->setTextAlignment(Qt::AlignCenter);
        name_item->setFlags(name_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, name_item);

        QTableWidgetItem* type_item = new QTableWidgetItem();
        type_item->setText(tunnel_item_info.type);
        type_item->setToolTip(tunnel_item_info.type);
        type_item->setTextAlignment(Qt::AlignCenter);
        type_item->setFlags(type_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, type_item);

        // 查找结点名称
        NodeInfo node_info;
        QString node_name = "";
        SFLGlobalMgr::GetInstance()->GetNodeInfo(node_info);
        for (auto node_item : node_info.data) {
            if (node_item.id == tunnel_item_info.node) {
                node_name = node_item.name;
            }
        }
        QString node_str = "#" + QString::number(tunnel_item_info.node) + " " + node_name;
        QTableWidgetItem* node_item = new QTableWidgetItem();
        node_item->setText(node_str);
        node_item->setToolTip(node_str);
        node_item->setTextAlignment(Qt::AlignCenter);
        node_item->setFlags(node_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, node_item);

        QTableWidgetItem* description_item = new QTableWidgetItem();
        description_item->setText(tunnel_item_info.description);
        description_item->setToolTip(tunnel_item_info.description);
        description_item->setTextAlignment(Qt::AlignCenter);
        description_item->setFlags(description_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, description_item);

        QTableWidgetItem* startup_time_item = new QTableWidgetItem();
        startup_time_item->setText(invalid_symbol);
        startup_time_item->setToolTip(invalid_symbol);
        startup_time_item->setTextAlignment(Qt::AlignCenter);
        startup_time_item->setFlags(startup_time_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, startup_time_item);

        QTableWidgetItem* running_time_item = new QTableWidgetItem();
        running_time_item->setText(invalid_symbol);
        running_time_item->setToolTip(invalid_symbol);
        running_time_item->setTextAlignment(Qt::AlignCenter);
        running_time_item->setFlags(running_time_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, running_time_item);

        QTableWidgetItem* process_status_item = new QTableWidgetItem();
        process_status_item->setText(QStringLiteral("停止"));
        process_status_item->setTextColor(Qt::red);
        process_status_item->setToolTip(QStringLiteral("停止"));
        process_status_item->setTextAlignment(Qt::AlignCenter);
        process_status_item->setFlags(process_status_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, process_status_item);

        QTableWidgetItem* running_state_item = new QTableWidgetItem();
        running_state_item->setText(invalid_symbol);
        running_state_item->setToolTip(invalid_symbol);
        running_state_item->setTextAlignment(Qt::AlignCenter);
        running_state_item->setFlags(running_state_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, running_state_item);

        QWidget* container_widget = new QWidget();
        QHBoxLayout* container_widget_h_layout = new QHBoxLayout(container_widget);
        container_widget_h_layout->setMargin(0);

        QPushButton* start_stop_btn = new QPushButton(container_widget);
        start_stop_btn->setObjectName("SFLTunnelTableWidget_start_stop_btn");
        start_stop_btn->setFixedSize(16, 16);
        start_stop_btn->setToolTip(QStringLiteral("启动/停止"));
        start_stop_btn->setCheckable(true);
        start_stop_btn->setCursor(Qt::PointingHandCursor);
        connect(start_stop_btn, SIGNAL(clicked()), m_start_stop_mapper, SLOT(map()));
        m_start_stop_mapper->setMapping(start_stop_btn, tunnel_item_info.tunnel_id);

        QPushButton* check_log_btn = new QPushButton(container_widget);
        check_log_btn->setObjectName("SFLTunnelTableWidget_check_log_btn");
        check_log_btn->setFixedSize(16, 16);
        check_log_btn->setToolTip(QStringLiteral("查看日志"));
        check_log_btn->setCursor(Qt::PointingHandCursor);
        connect(check_log_btn, SIGNAL(clicked()), m_check_log_mapper, SLOT(map()));
        m_check_log_mapper->setMapping(check_log_btn, tunnel_item_info.tunnel_id);

        container_widget_h_layout->addStretch();
        container_widget_h_layout->addWidget(start_stop_btn);
        container_widget_h_layout->addSpacing(10);
        container_widget_h_layout->addWidget(check_log_btn);
        container_widget_h_layout->addStretch();
        container_widget->setLayout(container_widget_h_layout);
        this->setCellWidget(i, this->columnCount() - 1, container_widget);

        QProcess* process = new QProcess(this);
        connect(process, SIGNAL(readyRead()), this, SLOT(OnProcessOutput()));
        connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(OnProcessStateChanged(QProcess::ProcessState)));
        m_tunnel_process_hash[tunnel_item_info.tunnel_id].process = process;
        m_tunnel_process_hash[tunnel_item_info.tunnel_id].log_text = "";
        m_tunnel_process_hash[tunnel_item_info.tunnel_id].startup_time = invalid_symbol;
        m_tunnel_process_hash[tunnel_item_info.tunnel_id].tunnel_item_info = tunnel_item_info;
    }
}

void SFLTunnelTableWidget::OnStartStopBtnClicked(
    const int& tunnel_id
) {
    QProcess::ProcessState state = m_tunnel_process_hash[tunnel_id].process->state();
    if (QProcess::NotRunning == m_tunnel_process_hash[tunnel_id].process->state()) {
        QString token = "";
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->GetValueByKey(db, "sfl_token", token);
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);

        QString app_dir_path = QDir::toNativeSeparators(QApplication::instance()->applicationDirPath());
        QString start_parameter = "\"" + app_dir_path + "\\frpc_windows_386.exe" + "\"" + " -f " + token + ":" + QString::number(tunnel_id);
        m_tunnel_process_hash[tunnel_id].process->start(start_parameter);
        m_tunnel_process_hash[tunnel_id].process->waitForStarted();
        m_tunnel_process_hash[tunnel_id].startup_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
// 
//         m_log_dlg->UpdateLog(m_tunnel_process_hash[tunnel_id]);
//         m_log_dlg->show();
    } else if (QProcess::Running == m_tunnel_process_hash[tunnel_id].process->state()) {
        m_tunnel_process_hash[tunnel_id].process->terminate();
        m_tunnel_process_hash[tunnel_id].process->kill();
        m_tunnel_process_hash[tunnel_id].process->waitForFinished();
        m_tunnel_process_hash[tunnel_id].startup_time = invalid_symbol;
        m_tunnel_process_hash[tunnel_id].running_state = e_running_state_none;
    } else if (QProcess::Starting == m_tunnel_process_hash[tunnel_id].process->state()) {
        m_tunnel_process_hash[tunnel_id].process->terminate();
        m_tunnel_process_hash[tunnel_id].process->kill();
        m_tunnel_process_hash[tunnel_id].process->waitForFinished();
        m_tunnel_process_hash[tunnel_id].startup_time = invalid_symbol;
        m_tunnel_process_hash[tunnel_id].running_state = e_running_state_none;
    }
    UpdateTable();
}

void SFLTunnelTableWidget::OnCheckLogBtnClicked(
    const int& tunnel_id
) {
    m_log_dlg->UpdateLog(m_tunnel_process_hash[tunnel_id]);
    m_log_dlg->show();
}

void SFLTunnelTableWidget::OnProcessOutput(
) {
    int tunnel_id = 0;
    QProcess* process = dynamic_cast<QProcess*>(this->sender());
    for (auto key : m_tunnel_process_hash.keys()) {
        if (m_tunnel_process_hash[key].process == process) {
            tunnel_id = key;
            break;
        }
    }
    QString text = process->readAll();
    m_tunnel_process_hash[tunnel_id].log_text += text;
    if (m_log_dlg->isVisible() && m_log_dlg->TunnelID() == tunnel_id) {
        m_log_dlg->UpdateLog(m_tunnel_process_hash[tunnel_id]);
    }

    // 判断输出日志级别
    if (-1 != text.indexOf(" [I] ") && -1 != text.indexOf("start proxy success")) {
        m_tunnel_process_hash[tunnel_id].running_state = e_running_state_info;
    } else if (-1 != text.indexOf(" [W] ")) {
        m_tunnel_process_hash[tunnel_id].running_state = e_running_state_warnning;
        SFLGlobalMgr::GetInstance()->Launcher()->ShowTrayMessage(m_tunnel_process_hash[tunnel_id]);
    } else if (-1 != text.indexOf(" [E] ")) {
        m_tunnel_process_hash[tunnel_id].running_state = e_running_state_error;
        SFLGlobalMgr::GetInstance()->Launcher()->ShowTrayMessage(m_tunnel_process_hash[tunnel_id]);
    }
//     else {
//         m_tunnel_process_hash[tunnel_id].running_state = e_running_state_none;
//     }
    UpdateTable();
}

void SFLTunnelTableWidget::OnProcessStateChanged(
    QProcess::ProcessState state
) {
    int process_tunnel_id = 0;
    QProcess* process = dynamic_cast<QProcess*>(this->sender());
    for (auto key : m_tunnel_process_hash.keys()) {
        if (m_tunnel_process_hash[key].process == process) {
            process_tunnel_id = key;
            break;
        }
    }
    for (int i = 0; i < this->rowCount(); ++i) {
        QTableWidgetItem* tunnel_id_item = this->item(i, 1);
        int tunnel_id = tunnel_id_item->text().toInt();

        if (process_tunnel_id == tunnel_id) {
            QWidget* container_widget = this->cellWidget(i, 9);
            if (nullptr != container_widget) {
                QPushButton* start_stop_btn = container_widget->findChild<QPushButton*>("SFLTunnelTableWidget_start_stop_btn");
                if (nullptr != start_stop_btn) {
                    if (QProcess::Running == process->state()) {
                        start_stop_btn->setChecked(true);
                    } else {
                        start_stop_btn->setChecked(false);
                    }
                }
            }
            break;
        }
    }
}

void SFLTunnelTableWidget::UpdateTable(
) {
    for (int i = 0; i < this->rowCount(); ++i) {
        QTableWidgetItem* tunnel_id_item = this->item(i, 1);
        int tunnel_id = tunnel_id_item->text().toInt();

        this->item(i, 6)->setText(m_tunnel_process_hash[tunnel_id].startup_time);

        if (invalid_symbol != m_tunnel_process_hash[tunnel_id].startup_time) {
            QDateTime startup_time = QDateTime::fromString(m_tunnel_process_hash[tunnel_id].startup_time, "yyyy-MM-dd hh:mm:ss");
            QString start_time = startup_time.toString("yyyy-MM-dd hh:mm:ss");
            QDateTime current_time = QDateTime::currentDateTime();
            QTime time;
            time.setHMS(0, 0, 0, 0);
            QString running_time = time.addSecs(startup_time.secsTo(current_time)).toString("hh:mm:ss");
            this->item(i, 7)->setText(running_time);
        } else {
            this->item(i, 7)->setText(invalid_symbol);
        }

        if (QProcess::Running != m_tunnel_process_hash[tunnel_id].process->state()) {
            this->item(i, 8)->setText(QStringLiteral("停止"));
            this->item(i, 8)->setTextColor(Qt::red);
        } else {
            this->item(i, 8)->setText(QStringLiteral("运行"));
            this->item(i, 8)->setTextColor(Qt::green);
        }

        if (e_running_state_none == m_tunnel_process_hash[tunnel_id].running_state) {
            this->item(i, 9)->setText(invalid_symbol);
            this->item(i, 9)->setTextColor(Qt::black);
        } else if (e_running_state_info == m_tunnel_process_hash[tunnel_id].running_state) {
            this->item(i, 9)->setText(QStringLiteral("正常"));
            this->item(i, 9)->setTextColor(Qt::green);
        } else if (e_running_state_warnning == m_tunnel_process_hash[tunnel_id].running_state) {
            this->item(i, 9)->setText(QStringLiteral("警告"));
            this->item(i, 9)->setTextColor(QColor(255, 130, 50));
        } else if (e_running_state_error == m_tunnel_process_hash[tunnel_id].running_state) {
            this->item(i, 9)->setText(QStringLiteral("错误"));
            this->item(i, 9)->setTextColor(Qt::red);
        }
    }
}

void SFLTunnelTableWidget::OnTimeout(
) {
    UpdateTable();
}