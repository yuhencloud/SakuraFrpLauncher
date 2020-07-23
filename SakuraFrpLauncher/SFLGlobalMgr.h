#pragma once
#ifndef SFLGLOBALMGR_H
#define SFLGLOBALMGR_H

#include <QObject>

#include "SFLLoadingDlg.h"
#include "SFLCommonDefs.h"

class SFLGlobalMgr : public QObject
{
    Q_OBJECT

public:

    static SFLGlobalMgr* GetInstance(
    );

    void DeleteInstance(
    );

public:

    void SetLoadingDlg(
        SFLLoadingDlg* loading_dlg
    );

    SFLLoadingDlg* LoadingDlg(
    ) const ;

public:
    
    void SetNodeInfo(
        const NodeInfo& node_info
    );

    void GetNodeInfo(
        NodeInfo& node_info
    ) const ;

    void SetTunnelInfo(
        const TunnelInfo& tunnel_info
    );

    void GetTunnelInfo(
        TunnelInfo& tunnel_info
    ) const ;

private:

    SFLGlobalMgr();

    ~SFLGlobalMgr();

private:

    static SFLGlobalMgr* m_global_mgr_instance;

    SFLLoadingDlg* m_loading_dlg;
    NodeInfo m_node_info;
    TunnelInfo m_tunnel_info;

};

#endif // SFLGLOBALMGR_H