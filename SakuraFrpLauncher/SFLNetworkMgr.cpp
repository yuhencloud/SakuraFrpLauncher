#include "SFLNetworkMgr.h"
#include <QEventLoop>
#include "SFLNetworkAccessManager.h"
#include <QFile>
#include <QDir>
#include <QHttpMultiPart>
#include "QThread.h"
#include <qt_windows.h>

SFLNetworkMgr::SFLNetworkMgr(
):
    m_result("")
{
    m_event_loop = new QEventLoop(this);
    m_network_access_manager = new SFLNetworkAccessManager(this);
    m_network_access_manager->SetNetworkMgr(this);
    m_result = "";
}

SFLNetworkMgr::~SFLNetworkMgr(
) {
    m_result = "";
}

NetworkState SFLNetworkMgr::PostData( 
    const QString& url, 
    const QString& msg_to_send, 
    QString& result, 
    int timeout /* = -1 */ ,
    QEventLoop::ProcessEventsFlags flags/* = QEventLoop::AllEvents*/
) {
    OutputDebugStringA(("@@@(Post)" + url).toLocal8Bit().toStdString().c_str());
    m_network_access_manager->PostData(url, msg_to_send, timeout);
    int code = m_event_loop->exec(flags); 
    result = m_result;
    OutputDebugStringA(("@@@(Post)" + result).toLocal8Bit().toStdString().c_str());
    NetworkState network_state = e_network_success;
    if (200 == code) {
        network_state = e_network_success;
    } else if (-1 == code) {
        network_state = e_network_handle_cancel;
    } else {
        network_state = e_network_fail;
    }
    return network_state;
}

NetworkState SFLNetworkMgr::GetData( 
    QString url, 
    QString& result, 
    int timeout /* = -1 */,
    QEventLoop::ProcessEventsFlags flags/* = QEventLoop::AllEvents*/
) {
    OutputDebugStringA(("@@@(Get)" + url).toLocal8Bit().toStdString().c_str());
    m_network_access_manager->GetData(url, timeout);
    int code = m_event_loop->exec(flags); 
    result = m_result;
    OutputDebugStringA(("@@@(Get)" + result).toLocal8Bit().toStdString().c_str());
    NetworkState network_state = e_network_success;
    if (200 == code) {
        network_state = e_network_success;
    } else if (-1 == code) {
        network_state = e_network_handle_cancel;
    } else {
        network_state = e_network_fail;
    }
    return network_state;
}

void SFLNetworkMgr::QuitEvent( 
    int code, 
    QString result 
) {
    m_result = result;
    m_event_loop->exit(code);
}

void SFLNetworkMgr::HandleCancel(
) {
    this->QuitEvent(-1, "");
}

SFLNetworkAccessManager* SFLNetworkMgr::NetworkAccessManager(
) {
    return m_network_access_manager;
}