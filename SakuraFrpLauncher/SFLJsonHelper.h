#pragma once
#ifndef SFLJSONHELPER_H
#define SFLJSONHELPER_H

#include <QObject>

#include "SFLCommonDefs.h"

class SFLJsonHelper : public QObject
{
    Q_OBJECT

public:

    SFLJsonHelper(
    );

    ~SFLJsonHelper(
    );

public:

    bool ParseNodesStringToStruct(
        const QString& nodes_json,
        NodeInfo& node_info
    );

    bool ParseTunnelsStringToStruct(
        const QString& tunnels_json,
        TunnelInfo& tunnel_info
    );

    bool ParseCreateTunnelStringToStruct(
        const QString& create_tunnel_json,
        CreateTunnelInfo& create_tunnel_info
    );

    bool ParseDeleteTunnelStringToStruct(
        const QString& delete_tunnel_json,
        DeleteTunnelInfo& delete_tunnel_info
    );

private:

    

};

#endif // SFLJSONHELPER_H
