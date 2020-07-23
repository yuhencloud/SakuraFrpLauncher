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

const QString invalid_symbol = "-";

using NetworkState = enum {
    e_network_success = 0,
    e_network_fail = e_network_success + 1,
    e_network_handle_cancel = e_network_fail + 1
};

using NodeItemInfo = struct {
    int id;
    QString name;
    bool accept_new;
    QString group_id;
};

using NodeInfo = struct {
    bool success;
    QString message;
    QVector<NodeItemInfo> data;
};

using TunnelItemInfo = struct {
    int tunnel_id;
    int tunnel_index;
    QString name;
    QString type;
    int node;
    QString description;
    QString group_id;
};

using TunnelInfo = struct {
    bool success;
    QString message;
    QVector<TunnelItemInfo> data;
};

using GroupItemInfo = struct {
    QString group_id;
    QString name;
};

using TunnelProcess = struct {
    QProcess* process;
    QString startup_time;
    QString log_text;
    TunnelItemInfo tunnel_item_info;
};

#endif // SFLCOMMONDEFS_H