//
// Created by lightquantum on 2020/12/26.
//

#include "ETAPrediction.h"
#include "Database.h"
#include <QLineF>
#include <QSqlQuery>
#include <QDateTime>
#include <cmath>
#include "MeanShift.h"
#include <queue>

qreal calcDist(TravelPoint p1, TravelPoint p2) {
    qreal x = abs(p1.departTime - p2.departTime);
    qreal y = QLineF(p1.departCoord, p2.departCoord).length();
    qreal z = QLineF(p1.arrivalCoord, p2.arrivalCoord).length();

    qreal norm_y = y * 5000;
    qreal norm_z = z * 5000;

    return x + norm_y + norm_z;
}

// TaskETAPrediction(int32 departTime, QPointF DepartCoord, QPointF ArrivalCoord) -> qint32
void TaskETAPrediction::run() {
    auto departTime = args.at(0).toInt();
    auto departCoord = args.at(1).toPointF();
    auto arrivalCoord = args.at(2).toPointF();

    // tp stands for TravelPoint
    TravelPoint tpTarget{departTime, departCoord, arrivalCoord};

    emit show_progress(false);

    Database dbConn;
    auto db = dbConn.get();

    if (!db) {
        emit task_complete(TaskError("No available database."));
        return;
    }

    emit message_change("Querying database");

    auto where_clause = QString("orig_lng > %1 and orig_lng < %2 "
                                "and orig_lat > %3 and orig_lat < %4 "
                                "and dest_lng > %5 and dest_lng < %6 "
                                "and dest_lat > %7 and dest_lat < %8")
            .arg(departCoord.x() - 0.05).arg(departCoord.x() + 0.05)
            .arg(departCoord.y() - 0.05).arg(departCoord.y() + 0.05)
            .arg(arrivalCoord.x() - 0.05).arg(arrivalCoord.x() + 0.05)
            .arg(arrivalCoord.y() - 0.05).arg(arrivalCoord.y() + 0.05);

    QSqlQuery query(*db);
    if (!query.exec(QString("select COUNT(*) from orders "
                            "where (%1)").arg(where_clause))) {
        emit task_complete(TaskError("Unable to query database."));
        return;
    }
    query.next();
    qint64 count = query.value(0).toLongLong();

    if (!query.exec(QString("select departure_time,end_time,orig_lng,orig_lat,dest_lng,dest_lat from orders "
                            "where (%1)").arg(where_clause))) {
        emit task_complete(TaskError("Unable to query database."));
        return;
    }

    QVector<QPointF> predictSpace{};

    emit show_progress(true);
    qreal lastProgress = 0;

    qint64 counter = 0;
    while (query.next()) {

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

        auto departureTime =
                QDateTime::fromSecsSinceEpoch(query.value(0).toInt()).time().msecsSinceStartOfDay() / 1000;
        auto endTime = QDateTime::fromSecsSinceEpoch(query.value(1).toInt()).time().msecsSinceStartOfDay() / 1000;
        auto refDepartCoord = QPointF(query.value(2).toDouble(), query.value(3).toDouble());
        auto refArrivalCoord = QPointF(query.value(4).toDouble(), query.value(5).toDouble());
        TravelPoint tpRef{departureTime, refDepartCoord, refArrivalCoord};

        qreal duration = endTime - departTime;

        auto dist = calcDist(tpTarget, tpRef);

        if (dist < 3000) {
            predictSpace.append(QPointF{dist, duration});
        }

        counter++;
    }

    auto cmp = [](QPointF p1, QPointF p2) { return p1.x() > p2.x(); };
    priority_queue<QPointF, vector<QPointF>, decltype(cmp)> queueTrunc(cmp);
    for (auto p: predictSpace) {
        queueTrunc.push(p);
    }

    QVector<QPointF> truncPredictSpace{};
    for (int i = 0; i < min(queueTrunc.size(), 100ul); i++) {
        truncPredictSpace.append(queueTrunc.top());
        queueTrunc.pop();
    }

    emit message_change("MeanShift Clustering");
    MeanShiftClass meanShift(50, truncPredictSpace);
    meanShift.DoMeanShiftCluster();

    QMap<qint32, QPointF> clusters;
    QMap<qint32, qint32> count_per_cluster;
    for (auto point: meanShift.dataset) {
        if (!(clusters.contains(point.label))) {
            clusters.insert(point.label, point.pos);
            count_per_cluster.insert(point.label, 1);
        } else {
            count_per_cluster[point.label]++;
        }
    }
    int bestIdx = 0;
    int bestDist = 0;
    for (int i = 0; i < count_per_cluster.count(); i++) {
        if (count_per_cluster[i] > bestDist) {
            bestDist = count_per_cluster[i];
            bestIdx = i;
        }
    }

    if (count_per_cluster[bestIdx] == 0) {
        emit task_complete(-1);
    } else {
        if (clusters[bestIdx].x() > 600){
            emit task_complete(-1);
        } else {
            emit task_complete(clusters[bestIdx].y());
        }
    }

}
