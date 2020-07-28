#pragma once
#ifndef SFLNETWORKMGR_H
#define SFLNETWORKMGR_H

#include <QObject>
#include <QEventLoop>

#include "SFLCommonDefs.h"


class SFLNetworkAccessManager;
class QEventLoop;

class SFLNetworkMgr : public QObject
{

    Q_OBJECT

public:

    SFLNetworkMgr();
    ~SFLNetworkMgr();

public:

	NetworkState PostData(
        const QString& url,
        const QString& msg_to_send, 
        QString& result,
        int timeout = -1,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents
    );

	NetworkState GetData(
        QString url, 
        QString& result, 
        int timeout = -1,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents
    );

    void HandleCancel(
    );

	void QuitEvent(
        int code, 
        QString result
    );

private:

    SFLNetworkAccessManager* NetworkAccessManager(
    );

private:

    SFLNetworkAccessManager* m_network_access_manager;
	QEventLoop* m_event_loop;
	QString m_result;

};

#endif // SFLNETWORKMGR_H
