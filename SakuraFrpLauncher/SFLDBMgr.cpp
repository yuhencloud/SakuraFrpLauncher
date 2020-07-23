#include "SFLDBMgr.h"

#include <QSqlQuery>
#include <QDir>
#include <QFile>
#include <QVector>
#include <QVariant>

#include "SakuraFrpCommon.h"

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
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    bool success = false;

    success = query.exec("create table sfl_base( \
                        sfl_key varchar primary key, \
                        sfl_value varchar)");

    success = query.exec("create table sfl_group( \
                        group_id varchar primary key, \
                        group_name varchar)");

    success = query.exec("create table sfl_tunnel( \
                        tunnel_id integer primary key, \
                        tunnel_index integer, \
                        name varchar, \
                        type varchar, \
                        node integer, \
                        description varchar, \
                        group_id varchar)");

    query.exec("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('sfl_version', '1.0.0')");
    query.exec("INSERT INTO sfl_base(sfl_key, sfl_value) VALUES('sfl_token', '')");

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
    QString conn_name = SakuraFrpCommon::NextUuid();
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
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
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
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("UPDATE sfl_base SET sfl_value='%1' WHERE sfl_key='%2'").arg(value).arg(key));
    return true;
}

bool SFLDBMgr::GetGroupIDByTunnelID(
    QSqlDatabase db,
    const int& tunnel_id,
    QString& group_id
) {
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("SELECT group_id FROM sfl_tunnel WHERE tunnel_id=%1").arg(tunnel_id));
    while(query.next()) {
        group_id = query.value(0).toString();
    }
    return true;
}

bool SFLDBMgr::GetGroupInfoByGroupID(
    QSqlDatabase db,
    const QString& group_id,
    GroupItemInfo& group_item_info
) {
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("SELECT group_id, group_name FROM sfl_group WHERE group_id='%1'").arg(group_id));
    while (query.next()) {
        group_item_info.group_id = query.value(0).toString();
        group_item_info.name = query.value(1).toString();
    }
    return true;
}

bool SFLDBMgr::DeleteGroup(
    QSqlDatabase db
) {
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("DELETE FROM sfl_group"));
    return true;
}

bool SFLDBMgr::DeleteTunnel(
    QSqlDatabase db
) {
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("DELETE FROM sfl_tunnel"));
    return true;
}

bool SFLDBMgr::InsertGroupAndTunnel(
    QSqlDatabase db,
    const QVector<GroupItemInfo>& group_item_info_list,
    const QVector<TunnelItemInfo>& tunnel_item_info_list
) {
    QSqlQuery query_group = QSqlQuery::QSqlQuery(db);
    query_group.prepare("insert into sfl_group values(?, ?)");
    QVector<QVariantList> group_list;
    group_list.resize(2);

    QSqlQuery query_tunnel = QSqlQuery::QSqlQuery(db);
    query_tunnel.prepare("insert into sfl_tunnel values(?, ?, ?, ?, ?, ?, ?)");
    QVector<QVariantList> tunnel_list;
    tunnel_list.resize(7);

    for (auto group_item_info : group_item_info_list) {
        group_list[0].push_back(group_item_info.group_id);
        group_list[1].push_back(group_item_info.name);
    }

    for (auto tunnel_item_info : tunnel_item_info_list) {
        tunnel_list[0].push_back(tunnel_item_info.tunnel_id);
        tunnel_list[1].push_back(tunnel_item_info.tunnel_index);
        tunnel_list[2].push_back(tunnel_item_info.name);
        tunnel_list[3].push_back(tunnel_item_info.type);
        tunnel_list[4].push_back(tunnel_item_info.node);
        tunnel_list[5].push_back(tunnel_item_info.description);
        tunnel_list[6].push_back(tunnel_item_info.group_id);
    }

    for (auto group : group_list) {
        query_group.addBindValue(group);
    }
    if (!query_group.execBatch()) {
        return false;
    }

    for (auto tunnel : tunnel_list) {
        query_tunnel.addBindValue(tunnel);
    }
    if (!query_tunnel.execBatch()) {
        return false;
    }

    return true;
}

bool SFLDBMgr::GetGroupInfo(
    QSqlDatabase db,
    QVector<GroupItemInfo>& group_item_info_list
) {
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("SELECT group_id, group_name FROM sfl_group"));
    while (query.next()) {
        GroupItemInfo group_item_info;
        group_item_info.group_id = query.value(0).toString();
        group_item_info.name = query.value(1).toString();
        group_item_info_list.push_back(group_item_info);
    }
    return true;
}

bool SFLDBMgr::GetTunnelInfoByGroupID(
    QSqlDatabase db,
    const QString& group_id,
    QVector<TunnelItemInfo>& tunnel_item_info_list
) {
    QSqlQuery query = QSqlQuery::QSqlQuery(db);
    query.exec(QString("SELECT tunnel_id, tunnel_index, name, type, node, description, group_id FROM sfl_tunnel WHERE group_id='%1' ORDER BY tunnel_index").arg(group_id));
    while(query.next()) {
        TunnelItemInfo tunnel_item_info;
        tunnel_item_info.tunnel_id = query.value(0).toInt();
        tunnel_item_info.tunnel_index = query.value(1).toInt();
        tunnel_item_info.name = query.value(2).toString();
        tunnel_item_info.type = query.value(3).toString();
        tunnel_item_info.node = query.value(4).toInt();
        tunnel_item_info.description = query.value(5).toString();
        tunnel_item_info.group_id = query.value(6).toString();
        tunnel_item_info_list.push_back(tunnel_item_info);
    }
    return true;
}