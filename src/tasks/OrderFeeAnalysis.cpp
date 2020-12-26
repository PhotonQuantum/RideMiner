//
// Created by lightquantum on 2020/12/25.
//

#include "Database.h"
#include <QSqlQuery>
#include <cmath>
#include "OrderFeeAnalysis.h"
#include "utils.h"

void TaskOrderFeeAnalysis::run() {
    QList<qint32> feeBucket{};
    emit show_progress(false);

    auto cache = getPersistKV("order_fee");
    if (cache) {
        auto cacheString = cache.value();
        auto rawFeeBuckets = cacheString.split(',');

        for (const auto &rawFeeBucket: rawFeeBuckets) {
            feeBucket.append(rawFeeBucket.toInt());
        }

        emit task_complete(QVariant::fromValue(feeBucket));
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

        if (!query.exec("select fee from orders")) {
            emit task_complete(TaskError("Unable to query database."));
            return;
        }

        emit show_progress(true);
        qreal lastProgress = 0;

        qint64 counter = 0;
        while (query.next()) {
            auto fee = query.value(0).toInt();

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

            mustItem(feeBucket, fee / 2);
            feeBucket[fee / 2]++;

            counter++;
        }
    }

    QStringList strFeeBuckets{};
    for (auto origFeeBucket: feeBucket){
        strFeeBuckets.append(QString::number(origFeeBucket));
    }
    setPersistKV("order_fee", strFeeBuckets.join(','));

    emit task_complete(QVariant::fromValue(feeBucket));
}
