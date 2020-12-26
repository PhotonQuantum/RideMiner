//
// Created by lightquantum on 2020/12/26.
//

#include "RevenueAnalysis.h"
#include "Database.h"
#include "utils.h"
#include <QSqlQuery>

void TaskRevenueAnalysis::run() {
    QList<qreal> revenue{};
    emit show_progress(false);

    auto cache = getPersistKV("revenue");
    if (cache){
        auto cacheString = cache.value();
        auto rawRevenues = cacheString.split(',');

        for (const auto& rawRevenue: rawRevenues){
            revenue.append(rawRevenue.toDouble());
        }

        emit task_complete(QVariant::fromValue(revenue));
        return;
    }

    {
        Database dbConn;
        auto db = dbConn.get();

        if (!db) {
            emit task_complete(TaskError("No available database."));
            return;
        }

        emit message_change("Querying database");

        QSqlQuery query(*db);
        if (!query.exec("select COUNT(*) from orders")) {
            emit task_complete(TaskError("Unable to query database."));
        }
        query.next();
        qint64 count = query.value(0).toLongLong();

        if (!query.exec("select end_time, fee from orders")) {
            emit task_complete(TaskError("Unable to query database."));
            return;
        }

        emit show_progress(true);
        qreal lastProgress = 0;

        qint64 counter = 0;
        while (query.next()) {
            auto end_time = query.value(0).toInt();
            qreal fee = query.value(1).toDouble();

            if (counter % 50 == 0) {
                if (cancelled) {
                    emit task_complete(TaskError("Task cancelled."));
                    return;
                }

                auto progress = qreal(counter) / count * 100;
                if (progress - lastProgress >= 1) {
                    emit progress_change(progress);
                    emit message_change(QString("Progress %1%").arg(round(progress)));
                    lastProgress = progress;
                }
            }

            auto time = QDateTime::fromSecsSinceEpoch(end_time).time();
            auto hour = time.hour();
            mustItem(revenue, hour);
            revenue[hour] += fee;

            counter++;
        }
    }

    QStringList strRevenue{};
    for (auto origRevenue: revenue){
        strRevenue.append(QString::number(origRevenue, 'f', 2));
    }
    setPersistKV("revenue", strRevenue.join(','));

    emit task_complete(QVariant::fromValue(revenue));
}