#pragma once

#ifndef SFLNETWORKACCESSMANAGER_H
#define SFLNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "SFLNetworkMgr.h"

class QTimer;

class SFLNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:

    SFLNetworkAccessManager(
        QObject * parent = 0
    );
    ~SFLNetworkAccessManager();

public:

	void PostData(
        QString uri, 
        QString msg_to_send, 
        int timeout = -1
    );

	void GetData(
        QString url, 
        int timeout = -1
    );

    void SetNetworkMgr(
        SFLNetworkMgr* network_mgr
    );

private slots:

	void replyFinished(
        QNetworkReply* reply
    );

	void HandleTimeOut(
    );

private:

	QTimer* m_timer;
    QNetworkReply* m_reply;
    SFLNetworkMgr* m_network_mgr;

};

#endif // SFLNETWORKACCESSMANAGER_H