#include "SFLGlobalMgr.h"
#include <QApplication>

SFLGlobalMgr* SFLGlobalMgr::m_global_mgr_instance = nullptr;

SFLGlobalMgr::SFLGlobalMgr()
    : QObject(qApp),
    m_loading_dlg(nullptr)
{
}

SFLGlobalMgr::~SFLGlobalMgr(
) {
}

SFLGlobalMgr* SFLGlobalMgr::GetInstance(
) {
    if (nullptr == m_global_mgr_instance) {
        m_global_mgr_instance = new SFLGlobalMgr();
        if (nullptr == m_global_mgr_instance) {
            return nullptr;
        }
    }
    return m_global_mgr_instance;
}

void SFLGlobalMgr::DeleteInstance(
) {
    if (nullptr != m_global_mgr_instance) {
        delete m_global_mgr_instance;
        m_global_mgr_instance = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

void SFLGlobalMgr::SetLoadingDlg(
    SFLLoadingDlg* loading_dlg
) {
    m_loading_dlg = loading_dlg;
}

SFLLoadingDlg* SFLGlobalMgr::LoadingDlg(
) const {
    return m_loading_dlg;
}

//////////////////////////////////////////////////////////////////////////

void SFLGlobalMgr::SetNodeInfo(
    const NodeInfo& node_info
) {
    m_node_info = node_info;
}

void SFLGlobalMgr::GetNodeInfo(
    NodeInfo& node_info
) const {
    node_info = m_node_info;
}

void SFLGlobalMgr::SetTunnelInfo(
    const TunnelInfo& tunnel_info
) {
    m_tunnel_info = tunnel_info;
}

void SFLGlobalMgr::GetTunnelInfo(
    TunnelInfo& tunnel_info
) const {
    tunnel_info = m_tunnel_info;
}