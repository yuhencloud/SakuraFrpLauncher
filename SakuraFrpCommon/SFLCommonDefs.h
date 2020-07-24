#pragma once
#ifndef SFLCOMMONDEFS_H
#define SFLCOMMONDEFS_H

#include <QString>
#include <QVector>
#include <QProcess>

const QString sakura_frp_domain = "https://api.natfrp.com/";
const QString uri_get_nodes = "launcher/get_nodes?";
const QString uri_get_tunnels = "launcher/get_tunnels?";
const QString uri_create_tunnel = "launcher/create_tunnel?";
const QString uri_delete_tunnel = "launcher/delete_tunnel?";

const QString invalid_symbol = "--";

using NetworkState = enum {
    e_network_success = 0,
    e_network_fail = e_network_success + 1,
    e_network_handle_cancel = e_network_fail + 1
};

using RunningState = enum {
    e_running_state_none = 0,
    e_running_state_info = e_running_state_none + 1,
    e_running_state_warnning = e_running_state_info + 1,
    e_running_state_error = e_running_state_warnning + 1
};

using NodeItemInfo = struct {
    int node_id;
    QString node_name;
    int node_accept_new;
};

using NodeInfo = struct {
    bool success;
    QString message;
    QVector<NodeItemInfo> node_item_info_list;
};

using TunnelItemInfo = struct {
    int tunnel_id;
    int tunnel_index;
    QString tunnel_name;
    QString tunnel_type;
    QString tunnel_description;
    NodeItemInfo node_item_info;
};

using TunnelInfo = struct {
    bool success;
    QString message;
    QVector<TunnelItemInfo> tunnel_item_info_list;
};

using TunnelProcess = struct {
    QProcess* process;
    QString startup_time;
    QString log_text;
    RunningState running_state;
    TunnelItemInfo tunnel_item_info;
};

#endif // SFLCOMMONDEFS_H