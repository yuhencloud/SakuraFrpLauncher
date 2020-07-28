#pragma once
#ifndef SFLDBMGR_H
#define SFLDBMGR_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>

#include "SFLCommonDefs.h"

class SFLDBMgr : public QObject
{
    Q_OBJECT

public:

    static SFLDBMgr* GetInstance(
    );

    void DeleteInstance(
    );

public: // 数据库基础操作

    bool OpenLocalDB(
        QString db_path
    );

	void CloseLocalDB(
	);

    QSqlDatabase GetSqlConn(
    );

    void GiveBackSqlConn(
        QSqlDatabase db
    );

    bool BeginTransaction(
        QSqlDatabase db
    );

    bool EndTransaction(
        QSqlDatabase db
    );

public: // 接口

    bool GetValueByKey(
        QSqlDatabase db,
        const QString& key,
        QString& value
    );

    bool UpdateValueByKey(
        QSqlDatabase db,
        const QString& key,
        const QString& value
    );

    bool DeleteTunnel(
        QSqlDatabase db
    );

    bool InsertTunnel(
        QSqlDatabase db,
        const QVector<TunnelItemInfo>& tunnel_item_info_list
    );

    bool GetNodeInfoList(
        QSqlDatabase db,
		QVector<NodeItemInfo>& node_item_info_list
    );

    bool GetTunnelInfoByNodeID(
        QSqlDatabase db,
        const int& node_id,
        QVector<TunnelItemInfo>& tunnel_item_info_list
    );

private:

    SFLDBMgr();

    ~SFLDBMgr();

private:

    static QMutex m_mutex;
    static SFLDBMgr* m_db_instance;
    QString m_db_path;

};

#endif // SFLDBMGR_H