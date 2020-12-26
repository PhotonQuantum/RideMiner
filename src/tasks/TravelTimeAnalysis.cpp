//
// Created by lightquantum on 2020/12/25.
//

#include "Database.h"
#include <QSqlQuery>
#include <cmath>
#include "TravelTimeAnalysis.h"
#include "utils.h"


void TaskTravelTimeAnalysis::run() {
    QList<qint32> travelTime{};
    emit show_progress(false);

    auto cache = getPersistKV("travel_time");
    if (cache) {
        auto cacheString = cache.value();
        auto rawTravelTimes = cacheString.split(',');

        for (const auto &rawTravelTime: rawTravelTimes) {
            travelTime.append(rawTravelTime.toInt());
        }

        emit task_complete(QVariant::fromValue(travelTime));
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

        if (!query.exec("select departure_time, end_time from orders")) {
            emit task_complete(TaskError("Unable to query database."));
            return;
        }

        emit show_progress(true);
        qreal lastProgress = 0;

        qint64 counter = 0;
        while (query.next()) {
            auto departure_time = query.value(0).toInt();
            auto end_time = query.value(1).toInt();

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

            auto timeDelta = (end_time - departure_time) / 60;
            mustItem(travelTime, timeDelta / 5);
            travelTime[timeDelta / 5]++;

            counter++;
        }
    }

    QStringList strTravelTimes{};
    for (auto origTravelTime: travelTime){
        strTravelTimes.append(QString::number(origTravelTime));
    }
    setPersistKV("travel_time", strTravelTimes.join(','));

    emit task_complete(QVariant::fromValue(travelTime));
}
