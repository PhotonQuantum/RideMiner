//
// Created by lightquantum on 2020/12/23.
//

#include "InitDatabase.h"
#include "../Database.h"
#include <QSqlQuery>

void TaskInitDatabase::run() {
    emit show_progress(false);

    Database dbConn;
    auto db = dbConn.get();
    if (!db) {
        emit task_complete(TaskError(QString("Failed to connect to database.")));
        return;
    }

    emit message_change("Initializing Database...");

    db->transaction();

    QSqlQuery query(*db);

    // orders
    auto ok = query.exec("create table if not exists orders (order_id TEXT PRIMARY KEY, departure_time INT, end_time INT, orig_lng REAL, orig_lat REAL, dest_lng REAL, dest_lat REAL, fee REAL) without rowid");
    if (!ok) {
        db->rollback();
        emit task_complete(TaskError(QString("Database initialization failed.")));
        return;
    }

    // kv
    ok = query.exec("create table if not exists kv (key TEXT PRIMARY KEY, value TEXT) without rowid");
    if (!ok) {
        db->rollback();
        emit task_complete(TaskError(QString("Database initialization failed.")));
        return;
    }

    // grids
    ok = query.exec("create table if not exists grids (grid_id INT PRIMARY KEY,vertex0_lng REAL,vertex0_lat REAL,vertex1_lng REAL,vertex1_lat REAL,vertex2_lng REAL,vertex2_lat REAL,vertex3_lng REAL,vertex3_lat REAL)");
    if (!ok) {
        db->rollback();
        emit task_complete(TaskError(QString("Database initialization failed.")));
        return;
    }

    if (!db->commit()){
        emit task_complete(TaskError(QString("Database initialization failed.")));
        return;
    }

    emit task_complete({});
}
