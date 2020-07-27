#include "SFLDBMgr.h"

#include <QSqlQuery>
#include <QDir>
#include <QFile>
#include <QVector>
#include <QVariant>

#include "SFLUtility.h"

QMutex SFLDBMgr::m_mutex;
SFLDBMgr* SFLDBMgr::m_db_instance = nullptr;

SFLDBMgr::SFLDBMgr()
    : QObject()
{
}

SFLDBMgr::~SFLDBMgr(
) {
	CloseLocalDB();
}

SFLDBMgr* SFLDBMgr::GetInstance(
) {
    if (nullptr == m_db_instance) {
        QMutexLocker locker(&m_mutex);
        if (nullptr == m_db_instance) {
            m_db_instance = new SFLDBMgr();
        }
    }
    return m_db_instance;
}

void SFLDBMgr::DeleteInstance(
) {
    if (nullptr != m_db_instance) {
        delete m_db_instance;
        m_db_instance = nullptr;
    }
}

bool SFLDBMgr::OpenLocalDB(
    QString db_path
) {
    QSqlDatabase db;
	m_db_path = QDir::toNativeSeparators(db_path);
    db = QSqlDatabase::addDatabase("QSQLITE", m_db_path);
    db.setDatabaseName(m_db_path);

    if (QFile::exists(m_db_path)) {
        return true;
    }
    if (!db.open()) {
        return false;
    }
    db.transaction();
    // create table
    QSqlQuery query(db);
    bool success = false;

    success = query.exec("create table sfl_base( \
                        sfl_key varchar primary key, \
                        sfl_value varchar)");

    success = query.exec("create table sfl_tunnel( \
                        tunnel_id integer primary key, \
                        tunnel_index integer, \
                        tunnel_name varchar, \
                        tunnel_type varchar, \
                        tunnel_description varchar, \
                        node_id integer, \
                        node_name varchar, \
                        node_accept_new integer)");

    query.exec(QString("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('%1', '1.0.0')").arg(sfl_version));
    query.exec(QString("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('%1', '')").arg(sfl_token));
    query.exec(QString("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('%1', '1')").arg(sfl_auto_get_tunnel));
    query.exec(QString("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('%1', '1')").arg(sfl_auto_start_process));
    query.exec(QString("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('%1', '1')").arg(sfl_tray_message));
    query.exec(QString("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('%1', '180000')").arg(sfl_auto_get_tunnel_time));

    if (!db.commit()) {
        return false;
    }
    db.close();
	return true;
}

void SFLDBMgr::CloseLocalDB(
) {
    m_db_path = "";
}

QSqlDatabase SFLDBMgr::GetSqlConn(
) {
    QString conn_name = SFLUtility::NextUuid();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", conn_name);
    db.setDatabaseName(m_db_path);
    db.open();
    return db;
}

void SFLDBMgr::GiveBackSqlConn( 
    QSqlDatabase db 
) {
    db.close();
}

bool SFLDBMgr::BeginTransaction(
    QSqlDatabase db
) {
    return db.transaction();
}

bool SFLDBMgr::EndTransaction(
    QSqlDatabase db
) {
    return db.commit();
}

bool SFLDBMgr::GetValueByKey(
    QSqlDatabase db,
    const QString& key,
    QString& value
) {
    QSqlQuery query(db);
    query.exec(QString("SELECT sfl_value FROM sfl_base where sfl_key='%1'").arg(key));
    while (query.next()) {
        value = query.value(0).toString();
    }
    return true;
}

bool SFLDBMgr::UpdateValueByKey(
    QSqlDatabase db,
    const QString& key,
    const QString& value
) {
    QSqlQuery query(db);
    query.exec(QString("UPDATE sfl_base SET sfl_value='%1' WHERE sfl_key='%2'").arg(value).arg(key));
    return true;
}

bool SFLDBMgr::DeleteTunnel(
    QSqlDatabase db
) {
    QSqlQuery query(db);
    query.exec(QString("DELETE FROM sfl_tunnel"));
    return true;
}

bool SFLDBMgr::InsertTunnel(
    QSqlDatabase db,
    const QVector<TunnelItemInfo>& tunnel_item_info_list
) {
    QSqlQuery query_tunnel(db);
    query_tunnel.prepare("insert into sfl_tunnel values(?, ?, ?, ?, ?, ?, ?, ?)");
    QVector<QVariantList> tunnel_list;
    tunnel_list.resize(8);

    for (auto tunnel_item_info : tunnel_item_info_list) {
        tunnel_list[0].push_back(tunnel_item_info.tunnel_id);
        tunnel_list[1].push_back(tunnel_item_info.tunnel_index);
        tunnel_list[2].push_back(tunnel_item_info.tunnel_name);
        tunnel_list[3].push_back(tunnel_item_info.tunnel_type);
        tunnel_list[4].push_back(tunnel_item_info.tunnel_description);
        tunnel_list[5].push_back(tunnel_item_info.node_item_info.node_id);
		tunnel_list[6].push_back(tunnel_item_info.node_item_info.node_name);
        tunnel_list[7].push_back(tunnel_item_info.node_item_info.node_accept_new);
    }

    for (auto tunnel : tunnel_list) {
        query_tunnel.addBindValue(tunnel);
    }
    if (!query_tunnel.execBatch()) {
        return false;
    }

    return true;
}

bool SFLDBMgr::GetNodeInfoList(
    QSqlDatabase db,
	QVector<NodeItemInfo>& node_item_info_list
) {
    QSqlQuery query(db);
    query.exec(QString("SELECT node_id, node_name, node_accept_new FROM sfl_tunnel GROUP BY node_id ORDER BY node_id"));
    while (query.next()) {
        NodeItemInfo node_item_info;
        node_item_info.node_id = query.value(0).toInt();
        node_item_info.node_name = query.value(1).toString();
        node_item_info.node_accept_new = query.value(2).toInt();
        node_item_info_list.push_back(node_item_info);
    }
    return true;
}

bool SFLDBMgr::GetTunnelInfoByNodeID(
    QSqlDatabase db,
    const int& node_id,
    QVector<TunnelItemInfo>& tunnel_item_info_list
) {
    QSqlQuery query(db);
    query.exec(QString("SELECT tunnel_id, tunnel_index, tunnel_name, tunnel_type, tunnel_description FROM sfl_tunnel WHERE node_id=%1 ORDER BY tunnel_index").arg(node_id));
    while(query.next()) {
        TunnelItemInfo tunnel_item_info;
        tunnel_item_info.tunnel_id = query.value(0).toInt();
        tunnel_item_info.tunnel_index = query.value(1).toInt();
        tunnel_item_info.tunnel_name = query.value(2).toString();
        tunnel_item_info.tunnel_type = query.value(3).toString();
        tunnel_item_info.tunnel_description = query.value(4).toString();
        tunnel_item_info_list.push_back(tunnel_item_info);
    }
    return true;
}