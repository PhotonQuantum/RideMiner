//
// Created by lightquantum on 2020/12/25.
//

#include "ChartDemand.h"
#include "utils.h"
#include <Database.h>
#include <QSqlQuery>

void TaskChartDemand::run() {
    auto spanLower = args.at(0).toInt();
    auto spanUpper = args.at(1).toInt();
    auto isDeparture = args.at(2).toBool();
    auto gridIdx = args.at(3).toInt();
    auto step = args.at(4).toInt();

    emit message_change("Querying database");
    emit show_progress(false);

    QString sql;
    if (gridIdx == -1) {
        sql = QString("select %1 from orders "
                      "where (%1 > %2 and %1 < %3) "
                      "order by %1")
                .arg(isDeparture ? "departure_time" : "end_time").arg(spanLower).arg(spanUpper);
    } else {
        auto _grids = getGrids();
        if (!_grids) {
            emit task_complete(TaskError("Unable to read grids."));
            return;
        }
        auto grids = _grids.value();

        auto grid = grids[gridIdx];

        sql = QString("select %1,%2,%3 from orders "
                      "where (%1 > %4 and %1 < %5 "
                      "and %2 > %6 and %2 < %7 "
                      "and %3 > %8 and %3 < %9) "
                      "order by %1")
                .arg(isDeparture ? "departure_time" : "end_time")
                .arg(isDeparture ? "orig_lng" : "dest_lng")
                .arg(isDeparture ? "orig_lat" : "dest_lat")
                .arg(spanLower).arg(spanUpper)
                .arg(grid.vertex0.longitude)
                .arg(grid.vertex2.longitude)
                .arg(grid.vertex2.latitude)
                .arg(grid.vertex0.latitude);
    }

    Database dbConn;
    auto db = dbConn.get();

    if (!db) {
        emit task_complete(TaskError("Unable to connect to database."));
        return;
    }

    QSqlQuery query(*db);
    if (!query.exec(sql)) {
        emit task_complete(TaskError("Unable to query database."));
        return;
    }

    QList<QLongIntPair> series;
    qint64 currentX = spanLower;
    auto counter = 0;
    qint64 orderCounter = 0;
    auto lastProgress = 0;
    emit show_progress(true);
    while (query.next()) {
        auto x = query.value(0).toInt();

        if (counter == 50) {
            if (cancelled) {
                emit task_complete(TaskError("Task cancelled."));
                return;
            }

            auto progress = qreal(x - spanLower) / (spanUpper - spanLower) * 100;
            if (progress - lastProgress >= 1) {
                emit progress_change(progress);
                emit message_change(QString("Progress %1%").arg(round(progress)));
                lastProgress = progress;
            }
            counter = 0;
        }

        orderCounter++;
        if (x >= currentX + step) {
            series.append({currentX*1000, orderCounter});

            orderCounter = 0;
            currentX += step;
            while (x >= currentX + step) {
                series.append({currentX*1000, 0});
                currentX += step;
            }
        }

        counter++;
    }
    series.append({currentX*1000, orderCounter});

    emit task_complete(QVariant::fromValue(series));
}
