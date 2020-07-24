#pragma once
#ifndef SFLTUNNELTABLEWIDGET_H
#define SFLTUNNELTABLEWIDGET_H

#include <QTableWidget>
#include <QMap>

#include "SFLCommonDefs.h"

class QSignalMapper;
class QProcess;
class QTimer;
class SFLLogDlg;

class SFLTunnelTableWidget : public QTableWidget
{
    Q_OBJECT

public:

    SFLTunnelTableWidget(QWidget *parent = 0);
    ~SFLTunnelTableWidget();

public:

    void InitTunnelTableWidget(
        const NodeItemInfo& node_item_info
    );

    void GetNodeItemInfo(
        NodeItemInfo& node_item_info
    );

    void TerminateAllProcess(
    );

private:

    void ClearTable(
    );

    void UpdateTable(
    );

private slots:

    void OnStartStopBtnClicked(
        const int& tunnel_id
    );

    void OnCheckLogBtnClicked(
        const int& tunnel_id
    );

    void OnProcessOutput(
    );

    void OnProcessStateChanged(
        QProcess::ProcessState state
    );

    void OnTimeout(
    );

private:

    NodeItemInfo m_node_item_info;
    QTimer* m_timer;
    QSignalMapper* m_start_stop_mapper;
    QSignalMapper* m_check_log_mapper;
    SFLLogDlg* m_log_dlg;
    QMap<int, TunnelProcess> m_tunnel_process_map;

};

#endif // SFLTUNNELTABLEWIDGET_H
