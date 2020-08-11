#include "SFLJsonHelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

SFLJsonHelper::SFLJsonHelper()
    : QObject()
{
    
}

SFLJsonHelper::~SFLJsonHelper(
) {

}

bool SFLJsonHelper::ParseNodesStringToStruct(
    const QString& nodes_json,
    NodeInfo& node_info
) {
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(nodes_json.toUtf8(), &json_error);
    if (json_error.error != QJsonParseError::NoError) {
        return false;
    }
    if (!parse_doucment.isObject()) {
        return false;
    }
    QJsonObject obj;
    obj = parse_doucment.object();

    node_info.success = obj.take("success").toBool();
    if (!node_info.success) {
        node_info.message = obj.take("message").toString();
        return true;
    }

    QJsonArray data_array = obj.take("data").toArray();
    for (auto node_item : data_array) {
        QJsonObject node_item_obj = node_item.toObject();

        NodeItemInfo node_item_info;
        node_item_info.node_id = node_item_obj.take("id").toInt();
        node_item_info.node_name = node_item_obj.take("name").toString();
        node_item_info.node_accept_new = static_cast<int>(node_item_obj.take("accept_new").toBool());
        node_info.node_item_info_list.push_back(node_item_info);
    }

    return true;
}

bool SFLJsonHelper::ParseTunnelsStringToStruct(
    const QString& tunnels_json,
    TunnelInfo& tunnel_info
) {
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(tunnels_json.toUtf8(), &json_error);
    if (json_error.error != QJsonParseError::NoError) {
        return false;
    }
    if (!parse_doucment.isObject()) {
        return false;
    }
    QJsonObject obj;
    obj = parse_doucment.object();

    tunnel_info.success = obj.take("success").toBool();
    if (!tunnel_info.success) {
        tunnel_info.message = obj.take("message").toString();
        return true;
    }
    QJsonArray data_array = obj.take("data").toArray();
    for (auto tunnel_item : data_array) {
        QJsonObject tunnel_item_obj = tunnel_item.toObject();

        TunnelItemInfo tunnel_item_info;
        tunnel_item_info.tunnel_id = tunnel_item_obj.take("id").toInt();
        tunnel_item_info.tunnel_name = tunnel_item_obj.take("name").toString();
        tunnel_item_info.tunnel_type = tunnel_item_obj.take("type").toString();
        tunnel_item_info.tunnel_description = tunnel_item_obj.take("description").toString();
        tunnel_item_info.node_item_info.node_id = tunnel_item_obj.take("node").toInt();

        tunnel_info.tunnel_item_info_list.push_back(tunnel_item_info);
    }

    return true;
}

bool SFLJsonHelper::ParseCreateTunnelStringToStruct(
    const QString& create_tunnel_json,
    CreateTunnelInfo& create_tunnel_info
) {
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(create_tunnel_json.toUtf8(), &json_error);
    if (json_error.error != QJsonParseError::NoError) {
        return false;
    }
    if (!parse_doucment.isObject()) {
        return false;
    }
    QJsonObject obj;
    obj = parse_doucment.object();

    create_tunnel_info.success = obj.take("success").toBool();
    if (!create_tunnel_info.success) {
        create_tunnel_info.message = obj.take("message").toString();
        return true;
    }
    QJsonObject data_obj = obj.take("data").toObject();
    create_tunnel_info.tunnel_item_info.tunnel_id = data_obj.take("id").toInt();
    create_tunnel_info.tunnel_item_info.tunnel_name = data_obj.take("name").toString();
    create_tunnel_info.tunnel_item_info.tunnel_type = data_obj.take("type").toString();
    create_tunnel_info.tunnel_item_info.tunnel_description = data_obj.take("description").toString();
    create_tunnel_info.tunnel_item_info.node_item_info.node_id = data_obj.take("node").toInt();
    return true;
}