//
// Created by lightquantum on 2020/12/23.
//

#include "Database.h"
#include "utils.h"

QMutex Database::muDB;

std::optional<QSqlDatabase> Database::get() {
    auto [_, filePath, _b] = getDatabasePath();

    auto db = QSqlDatabase::addDatabase("QSQLITE", "conn");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    db.setDatabaseName(QString("file:%1?cache=shared").arg(filePath));

    if (!db.open()){
        return {};
    }

    return db;
}

bool Database::createConnection() {
    auto [_, filePath, _b] = getDatabasePath();

    auto db = QSqlDatabase::addDatabase("QSQLITE", "globalConn");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    db.setDatabaseName(QString("file:%1?cache=shared").arg(filePath));

    bool isOpened = db.open();

    return isOpened;
}

void Database::closeConnection() {
    QSqlDatabase::removeDatabase("conn");
    QSqlDatabase::removeDatabase("globalConn");
}

Database::Database() {
    muDB.lock();
}

Database::~Database() {
    QSqlDatabase::removeDatabase("conn");
    muDB.unlock();
}
