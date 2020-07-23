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

private:

    

private:

    

};

#endif // SFLJSONHELPER_H
