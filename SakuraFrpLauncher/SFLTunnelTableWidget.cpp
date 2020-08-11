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
#include <QCheckBox>
#include <QHash>
#include <QPair>

#include "SFLDBMgr.h"
#include "SFLGlobalMgr.h"
#include "SFLLogDlg.h"
#include "SFLUtility.h"
#include "SFLMsgBox.h"
#include "SFLNetworkMgr.h"
#include "SFLJsonHelper.h"

SFLTunnelTableWidget::SFLTunnelTableWidget(QWidget *parent)
    : QTableWidget(parent),
    m_timer(nullptr),
    m_start_stop_mapper(nullptr),
    m_check_log_mapper(nullptr),
    m_log_dlg(nullptr)
{
    this->setColumnCount(10);
    QStringList header_label_list;
    header_label_list
        << QString::fromLocal8Bit("序号")
        << QString::fromLocal8Bit("隧道ID")
        << QString::fromLocal8Bit("隧道")
        << QString::fromLocal8Bit("类型")
        << QString::fromLocal8Bit("描述")
        << QString::fromLocal8Bit("启动时间")
        << QString::fromLocal8Bit("运行时间")
        << QString::fromLocal8Bit("进程状态")
        << QString::fromLocal8Bit("运行状态")
        << QString::fromLocal8Bit("操作");
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

void SFLTunnelTableWidget::ClearTable(
) {
    this->setRowCount(0);
    this->clearContents();
}

void SFLTunnelTableWidget::InitTunnelTableWidget(
    const NodeItemInfo& node_item_info
) {
    m_node_item_info = node_item_info;
    ClearTable();
    // 删除连接mapping
    if (nullptr != m_start_stop_mapper) {
        disconnect(m_start_stop_mapper, SIGNAL(mapped(int)), this, SLOT(OnStartStopBtnClicked(int)));
        QSignalMapper* temp_mapper = m_start_stop_mapper;
        temp_mapper->deleteLater();
        m_start_stop_mapper = nullptr;
    }
    if (nullptr != m_check_log_mapper) {
        disconnect(m_check_log_mapper, SIGNAL(mapped(int)), this, SLOT(OnCheckLogBtnClicked(int)));
        QSignalMapper* temp_mapper = m_check_log_mapper;
        temp_mapper->deleteLater();
        m_check_log_mapper = nullptr;
    }

    // 获取新的节点隧道列表
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    QVector<TunnelItemInfo> tunnel_item_info_list;
    SFLDBMgr::GetInstance()->GetTunnelInfoByNodeID(db, node_item_info.node_id, tunnel_item_info_list);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);

    if (!m_tunnel_process_map.isEmpty()) {
        for (auto key : m_tunnel_process_map.keys()) {
            bool exists = false;
            TunnelItemInfo new_tunnel_item_info;
            for (auto tunnel_item_info : tunnel_item_info_list) {
                if (m_tunnel_process_map[key].tunnel_item_info.tunnel_id == tunnel_item_info.tunnel_id) {
                    new_tunnel_item_info = tunnel_item_info;
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                StopProcess(key);
                delete m_tunnel_process_map[key].process;
                m_tunnel_process_map[key].process = nullptr;
                m_tunnel_process_map.remove(key);
            } else {
                m_tunnel_process_map[key].tunnel_item_info = new_tunnel_item_info;
            }
        }
    }

    for (auto tunnel_item_info : tunnel_item_info_list) {
        bool exists = false;
        for (auto key : m_tunnel_process_map.keys()) {
            if (m_tunnel_process_map[key].tunnel_item_info.tunnel_id == tunnel_item_info.tunnel_id) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            QProcess* process = new QProcess(this);
            connect(process, SIGNAL(readyRead()), this, SLOT(OnProcessOutput()));
            connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(OnProcessStateChanged(QProcess::ProcessState)));
            m_tunnel_process_map[tunnel_item_info.tunnel_id].process = process;
            m_tunnel_process_map[tunnel_item_info.tunnel_id].log_text = "";
            m_tunnel_process_map[tunnel_item_info.tunnel_id].startup_time = invalid_symbol;
            m_tunnel_process_map[tunnel_item_info.tunnel_id].running_state = e_running_state_none;
            m_tunnel_process_map[tunnel_item_info.tunnel_id].tunnel_item_info = tunnel_item_info;

            QString auto_start_process = "0";
            QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
            SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_auto_start_process, auto_start_process);
            SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
            if ("1" == auto_start_process) {
                StartProcess(tunnel_item_info.tunnel_id);
            }
        }
    }

    m_start_stop_mapper = new QSignalMapper();
    connect(m_start_stop_mapper, SIGNAL(mapped(int)), this, SLOT(OnStartStopBtnClicked(int)));
    m_check_log_mapper = new QSignalMapper();
    connect(m_check_log_mapper, SIGNAL(mapped(int)), this, SLOT(OnCheckLogBtnClicked(int)));

    this->setRowCount(tunnel_item_info_list.size());
    // 初始化列表
    for (int i = 0; i < m_tunnel_process_map.keys().size(); ++i) {
        int col_index = 0;
        TunnelItemInfo tunnel_item_info = m_tunnel_process_map[m_tunnel_process_map.keys()[i]].tunnel_item_info;

        QCheckBox* index_check_box = new QCheckBox(this);
        index_check_box->setText(QString::number(i + 1));
        this->setCellWidget(i, col_index++, index_check_box);

        QTableWidgetItem* tunnel_id_item = new QTableWidgetItem();
        QString tunnel_id = QString::number(tunnel_item_info.tunnel_id);
        tunnel_id_item->setText(tunnel_id);
        tunnel_id_item->setToolTip(tunnel_id);
        tunnel_id_item->setTextAlignment(Qt::AlignCenter);
        tunnel_id_item->setFlags(tunnel_id_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, tunnel_id_item);

        QTableWidgetItem* name_item = new QTableWidgetItem();
        name_item->setText(tunnel_item_info.tunnel_name);
        name_item->setToolTip(tunnel_item_info.tunnel_name);
        name_item->setTextAlignment(Qt::AlignCenter);
        name_item->setFlags(name_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, name_item);

        QTableWidgetItem* type_item = new QTableWidgetItem();
        type_item->setText(tunnel_item_info.tunnel_type);
        type_item->setToolTip(tunnel_item_info.tunnel_type);
        type_item->setTextAlignment(Qt::AlignCenter);
        type_item->setFlags(type_item->flags() & (~Qt::ItemIsEditable));
        this->setItem(i, col_index++, type_item);

        QTableWidgetItem* description_item = new QTableWidgetItem();
        description_item->setText(tunnel_item_info.tunnel_description);
        description_item->setToolTip(tunnel_item_info.tunnel_description);
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
        process_status_item->setText(QString::fromLocal8Bit("停止"));
        process_status_item->setTextColor(Qt::red);
        process_status_item->setToolTip(QString::fromLocal8Bit("停止"));
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
        start_stop_btn->setToolTip(QString::fromLocal8Bit("启动/停止"));
        start_stop_btn->setCheckable(true);
        start_stop_btn->setCursor(Qt::PointingHandCursor);
        connect(start_stop_btn, SIGNAL(clicked()), m_start_stop_mapper, SLOT(map()));
        m_start_stop_mapper->setMapping(start_stop_btn, tunnel_item_info.tunnel_id);

        QPushButton* check_log_btn = new QPushButton(container_widget);
        check_log_btn->setObjectName("SFLTunnelTableWidget_check_log_btn");
        check_log_btn->setFixedSize(16, 16);
        check_log_btn->setToolTip(QString::fromLocal8Bit("查看日志"));
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
    }
    UpdateTable();
}

void SFLTunnelTableWidget::GetNodeItemInfo(
    NodeItemInfo& node_item_info
) {
    node_item_info = m_node_item_info;
}

void SFLTunnelTableWidget::TerminateAllProcess(
) {
    for (auto key : m_tunnel_process_map.keys()) {
        StopProcess(key);
    }
}

void SFLTunnelTableWidget::OnStartStopBtnClicked(
    const int& tunnel_id
) {
    QProcess::ProcessState state = m_tunnel_process_map[tunnel_id].process->state();
    if (QProcess::NotRunning == m_tunnel_process_map[tunnel_id].process->state()) {
        StartProcess(tunnel_id);
    } else {
        StopProcess(tunnel_id);
    }
    UpdateTable();
}

void SFLTunnelTableWidget::OnCheckLogBtnClicked(
    const int& tunnel_id
) {
    m_log_dlg->UpdateLog(m_tunnel_process_map[tunnel_id]);
    m_log_dlg->show();
}

void SFLTunnelTableWidget::StartProcess(
    const int& tunnel_id
) {
    QString token = "";
    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
    SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_token, token);
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);

#ifdef Q_OS_WIN
    QString exe_name = "frpc_windows.exe";
#endif
#ifdef Q_OS_LINUX
    QString exe_name = "frpc_linux";
#endif
#ifdef Q_OS_MAC
    QString exe_name = "frpc_mac";
#endif
    QString exe_path = "\"" + QDir::toNativeSeparators(QApplication::instance()->applicationDirPath() + "/" + exe_name) + "\"";
    QString start_parameter = exe_path + " -f " + token + ":" + QString::number(tunnel_id);
    m_tunnel_process_map[tunnel_id].process->start(start_parameter);
    m_tunnel_process_map[tunnel_id].process->waitForStarted();
    m_tunnel_process_map[tunnel_id].startup_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

void SFLTunnelTableWidget::StopProcess(
    const int& tunnel_id
) {
    m_tunnel_process_map[tunnel_id].process->terminate();
    m_tunnel_process_map[tunnel_id].process->kill();
    m_tunnel_process_map[tunnel_id].process->waitForFinished();
    m_tunnel_process_map[tunnel_id].startup_time = invalid_symbol;
    m_tunnel_process_map[tunnel_id].log_text = "";
    m_tunnel_process_map[tunnel_id].running_state = e_running_state_none;
}

void SFLTunnelTableWidget::OnProcessOutput(
) {
    int tunnel_id = 0;
    QProcess* process = dynamic_cast<QProcess*>(this->sender());
    for (auto key : m_tunnel_process_map.keys()) {
        if (m_tunnel_process_map[key].process == process) {
            tunnel_id = key;
            break;
        }
    }
    QString text = process->readAll();
    m_tunnel_process_map[tunnel_id].log_text += text;
    if (m_log_dlg->isVisible() && m_log_dlg->TunnelID() == tunnel_id) {
        m_log_dlg->UpdateLog(m_tunnel_process_map[tunnel_id]);
    }

    // 判断输出日志级别
    if (-1 != text.indexOf("[I]") && -1 != text.indexOf("start proxy success")) {
        m_tunnel_process_map[tunnel_id].running_state = e_running_state_info;
    } else if (-1 != text.indexOf("[W]")) {
        m_tunnel_process_map[tunnel_id].running_state = e_running_state_warnning;
        SFLGlobalMgr::GetInstance()->Launcher()->ShowTrayMessage(m_tunnel_process_map[tunnel_id]);
    } else if (-1 != text.indexOf("[E]")) {
        m_tunnel_process_map[tunnel_id].running_state = e_running_state_error;
        SFLGlobalMgr::GetInstance()->Launcher()->ShowTrayMessage(m_tunnel_process_map[tunnel_id]);
    } else {

    }
    UpdateTable();
}

void SFLTunnelTableWidget::OnProcessStateChanged(
    QProcess::ProcessState state
) {
    int tunnel_id = 0;
    QProcess* process = dynamic_cast<QProcess*>(this->sender());
    for (auto key : m_tunnel_process_map.keys()) {
        if (m_tunnel_process_map[key].process == process) {
            tunnel_id = key;
            break;
        }
    }
    if (QProcess::Running != m_tunnel_process_map[tunnel_id].process->state()) {
        m_tunnel_process_map[tunnel_id].startup_time = invalid_symbol;
        m_tunnel_process_map[tunnel_id].log_text = "";
        m_tunnel_process_map[tunnel_id].running_state = e_running_state_none;
    }
    UpdateTable();
}

void SFLTunnelTableWidget::UpdateTable(
) {
    for (int i = 0; i < this->rowCount(); ++i) {
        QTableWidgetItem* tunnel_id_item = this->item(i, 1);
        int tunnel_id = tunnel_id_item->text().toInt();

        // 启动时间
        this->item(i, 5)->setText(m_tunnel_process_map[tunnel_id].startup_time);

        // 运行时间
        if (invalid_symbol != m_tunnel_process_map[tunnel_id].startup_time) {
            QDateTime startup_time = QDateTime::fromString(m_tunnel_process_map[tunnel_id].startup_time, "yyyy-MM-dd hh:mm:ss");
            QString start_time = startup_time.toString("yyyy-MM-dd hh:mm:ss");
            QDateTime current_time = QDateTime::currentDateTime();
            QTime time;
            time.setHMS(0, 0, 0, 0);
            QString running_time = time.addSecs(startup_time.secsTo(current_time)).toString("hh:mm:ss");
            this->item(i, 6)->setText(running_time);
        } else {
            this->item(i, 6)->setText(invalid_symbol);
        }

        // 进程状态
        if (QProcess::Running != m_tunnel_process_map[tunnel_id].process->state()) {
            this->item(i, 7)->setText(QString::fromLocal8Bit("停止"));
            this->item(i, 7)->setTextColor(Qt::red);
        } else {
            this->item(i, 7)->setText(QString::fromLocal8Bit("运行"));
            this->item(i, 7)->setTextColor(Qt::green);
        }

        // 运行状态
        if (e_running_state_none == m_tunnel_process_map[tunnel_id].running_state) {
            this->item(i, 8)->setText(invalid_symbol);
            this->item(i, 8)->setTextColor(Qt::black);
        } else if (e_running_state_info == m_tunnel_process_map[tunnel_id].running_state) {
            this->item(i, 8)->setText(QString::fromLocal8Bit("正常"));
            this->item(i, 8)->setTextColor(Qt::green);
        } else if (e_running_state_warnning == m_tunnel_process_map[tunnel_id].running_state) {
            this->item(i, 8)->setText(QString::fromLocal8Bit("警告"));
            this->item(i, 8)->setTextColor(QColor(255, 130, 50));
        } else if (e_running_state_error == m_tunnel_process_map[tunnel_id].running_state) {
            this->item(i, 8)->setText(QString::fromLocal8Bit("错误"));
            this->item(i, 8)->setTextColor(Qt::red);
        }

        // 按钮状态
        QWidget* container_widget = this->cellWidget(i, 9);
        if (nullptr != container_widget) {
            QPushButton* start_stop_btn = container_widget->findChild<QPushButton*>("SFLTunnelTableWidget_start_stop_btn");
            if (nullptr != start_stop_btn) {
                if (QProcess::Running == m_tunnel_process_map[tunnel_id].process->state()) {
                    start_stop_btn->setChecked(true);
                } else {
                    start_stop_btn->setChecked(false);
                }
            }
        }
    }
}

void SFLTunnelTableWidget::OnTimeout(
) {
    UpdateTable();
}

void SFLTunnelTableWidget::SetAllCheckBoxState(
    int state
) {
    for (int i = 0; i < this->rowCount(); ++i) {
        QCheckBox* index_chech_box = dynamic_cast<QCheckBox*>(this->cellWidget(i, 0));
        if (nullptr != index_chech_box) {
            index_chech_box->setCheckState(Qt::CheckState(state));
        }
    }
}

void SFLTunnelTableWidget::StartStopSelectedTunnel(
    bool start
) {
    for (int i = 0; i < this->rowCount(); ++i) {
        QCheckBox* index_chech_box = dynamic_cast<QCheckBox*>(this->cellWidget(i, 0));
        if (nullptr == index_chech_box) {
            continue;
        }
        if (!index_chech_box->isChecked()) {
            continue;
        }
        // 获取tunnel id
        int tunnel_id = this->item(i, 1)->text().toInt();
        if (start) {
            if (QProcess::NotRunning == m_tunnel_process_map[tunnel_id].process->state()) {
                // 当前未启动，则启动
                StartProcess(tunnel_id);
            }
        } else {
            if (QProcess::NotRunning != m_tunnel_process_map[tunnel_id].process->state()) {
                // 当前启动，则停止
                StopProcess(tunnel_id);
            }
        }
    }
    UpdateTable();
}

void SFLTunnelTableWidget::DeleteSelectedTunnel(
) {
    // 停止当前选中隧道
    StartStopSelectedTunnel(false);

    SFLLoadingDlg* login_dlg = SFLGlobalMgr::GetInstance()->LoadingDlg();
    login_dlg->SetText(QString::fromLocal8Bit("正在删除隧道..."));
    login_dlg->show();

    // 删除当前选中隧道
    QHash<int, QPair<bool, DeleteTunnelInfo>> tunnel_id_delete_tunnel_info_hash;
    for (int i = 0; i < this->rowCount(); ++i) {
        QCheckBox* index_chech_box = dynamic_cast<QCheckBox*>(this->cellWidget(i, 0));
        if (nullptr == index_chech_box) {
            continue;
        }
        if (!index_chech_box->isChecked()) {
            continue;
        }
        // 获取tunnel id
        int tunnel_id = this->item(i, 1)->text().toInt();
        QString delete_tunnel_url = sakura_frp_domain + uri_delete_tunnel;
        QString token = "";
        QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
        SFLDBMgr::GetInstance()->GetValueByKey(db, sfl_token, token);
        SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
        delete_tunnel_url +=
            "token=" + token
            + "&tunnel=" + QString::number(tunnel_id);
        QString delete_tunnel_json = "";
        SFLNetworkMgr().GetData(delete_tunnel_url, delete_tunnel_json, 60000);

        DeleteTunnelInfo delete_tunnel_info;
        bool delete_tunnel_suc = SFLJsonHelper().ParseDeleteTunnelStringToStruct(delete_tunnel_json, delete_tunnel_info);
        tunnel_id_delete_tunnel_info_hash[tunnel_id] = QPair<bool, DeleteTunnelInfo>(delete_tunnel_suc, delete_tunnel_info);
    }

    login_dlg->hide();

    // 组合弹出框
    SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
    QString text = "";
    bool is_success = true;
    QHash<int, QPair<bool, DeleteTunnelInfo>>::iterator it = tunnel_id_delete_tunnel_info_hash.begin();
    for (; it != tunnel_id_delete_tunnel_info_hash.end(); ++it) {
        if (it.value().first && it.value().second.success) {
            continue;
        }
        is_success = false;
        if (!it.value().first) {
            text += QString::number(it.key()) + ":" + QString::fromLocal8Bit("网络或服务器错误，请稍后重试") + "\t";
        } else {
            if (!it.value().second.success) {
                text += QString::number(it.key()) + ":" + it.value().second.message + "\t";
            }
        }
    }
    
    if (is_success) {
        text = QString::fromLocal8Bit("删除成功");
    } else {
        text = QString::fromLocal8Bit("删除失败") + "\t" + text;
    }

    SFLMsgBox::GetInstance()->setText(text);
    SFLMsgBox::GetInstance()->exec();

    // 刷新列表
    SFLGlobalMgr::GetInstance()->Launcher()->OnGetTunnelBtnClicked();
}