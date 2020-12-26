//
// Created by lightquantum on 2020/12/25.
//

#include "BasicStats.h"
#include <QSqlQuery>
#include <Database.h>
#include "types.h"

void TaskBasicStats::run() {
    emit show_progress(false);
    emit message_change("Querying database");

    Database dbConn;
    auto db = dbConn.get();

    if (!db) {
        emit task_complete(TaskError("No available database."));
        return;
    }

    QSqlQuery query(*db);

    if (!query.exec("select COUNT(*) from orders")){
        emit task_complete(TaskError("Unable to query database."));
    }
    query.next();
    qint64 count = query.value(0).toLongLong();

    if (!query.exec("select sum(fee) from orders")){
        emit task_complete(TaskError("Unable to query database."));
    }

    query.next();
    if (query.value(0).isNull()){
        emit task_complete(QVariant::fromValue(QLongIntRealPair{count, -1}));
    }

    qreal revenue = query.value(0).toDouble();

    emit task_complete(QVariant::fromValue(QLongIntRealPair {count, revenue}));
}