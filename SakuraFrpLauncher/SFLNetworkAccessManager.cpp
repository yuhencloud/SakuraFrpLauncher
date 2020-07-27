#include "SFLNetworkAccessManager.h"
#include "SFLNetworkMgr.h"

#include <QTimer>

#include "SFLUtility.h"

#define NET_TIMEOUT 999999

SFLNetworkAccessManager::SFLNetworkAccessManager(QObject* parent)
	: QNetworkAccessManager(parent),
    m_timer(nullptr),
    m_reply(nullptr)
{
	m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(HandleTimeOut()));
	connect(this,SIGNAL(finished(QNetworkReply *)),this,SLOT(replyFinished(QNetworkReply*)));
}

SFLNetworkAccessManager::~SFLNetworkAccessManager(
) {
    disconnect(m_timer, SIGNAL(timeout()), this, SLOT(HandleTimeOut()));
    disconnect(this,SIGNAL(finished(QNetworkReply *)),this,SLOT(replyFinished(QNetworkReply*)));
}

void SFLNetworkAccessManager::SetNetworkMgr( 
    SFLNetworkMgr* network_mgr 
) {
    m_network_mgr = network_mgr;
}

void SFLNetworkAccessManager::PostData(
    QString url,
    QString msg_to_send, 
    int timeout /* = -1 */ 
) {
	QNetworkRequest network_request;
	network_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	network_request.setHeader(QNetworkRequest::ContentLengthHeader, msg_to_send.length());
	network_request.setUrl(QUrl(url));
	m_reply = this->post(network_request, msg_to_send.toLatin1());
    if (-1 == timeout) {
        m_timer->start(NET_TIMEOUT);
    } else {
        m_timer->start(timeout);
    }	
}

void SFLNetworkAccessManager::GetData(
    QString url, 
    int timeout /* = -1 */ 
) {
	QNetworkRequest network_request;
	network_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	network_request.setUrl(QUrl(url));
	m_reply = this->get(network_request);
    if (-1 == timeout) {
        m_timer->start(NET_TIMEOUT);
    } else {
        m_timer->start(timeout);
    }
}

void SFLNetworkAccessManager::replyFinished(
    QNetworkReply* reply
) {
	m_timer->stop();
    if (nullptr != m_reply) {
        int status_code = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString reponseByte = m_reply->readAll();
        SFLUtility::UnicodeTransfrom(reponseByte);
        m_network_mgr->QuitEvent(status_code, reponseByte);
        m_reply->deleteLater();
        m_reply = nullptr;
    } else {
        m_network_mgr->QuitEvent(-1, "");
    }
}

void SFLNetworkAccessManager::HandleTimeOut(
) {
	m_timer->stop();
    if (nullptr != m_reply) {
        m_reply->close();
    }
}