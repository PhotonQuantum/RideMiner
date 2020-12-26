//
// Created by lightquantum on 2020/12/23.
//

#include "LoadDataset.h"
#include "../Database.h"
#include "../Config.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDir>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlError>
#include <types.h>
#include <cmath>

void TaskLoadDataset::run() {
    static const QStringList allFields{"order_id", "departure_time", "end_time", "orig_lng", "orig_lat", "dest_lng",
                                       "dest_lat", "fee"};

    QString datasetPath = args.first().toString();

    Database dbConn;
    auto db = dbConn.get();

    if (!db) {
        emit task_complete(TaskError("No available database."));
        return;
    }

    if (!db->transaction()) {
        emit task_complete(TaskError("Unable to initiate transaction."));
        return;
    }

    auto &configs = Config::getInstance();
    auto fields = configs.get("loadFields").value().toStringList();
    auto [spanLower, spanHigher] = configs.get("loadRange").value().value<QIntPair>();

    // =================================================================================================================
    // Load kv
    // =================================================================================================================
    {
        QSqlQuery query(*db);

        auto sql = "insert into kv (key,value) values (?,?)";
        query.prepare(sql);

        query.bindValue(0, "fields");
        query.bindValue(1, fields.join(','));
        if (!query.exec()){
            emit message_change("Reverting changes");
            db->rollback();

            emit task_complete(TaskError("Failed to insert row."));
            return;
        }

        query.bindValue(0, "range");
        query.bindValue(1, QString("%1,%2").arg(spanLower).arg(spanHigher));
        if (!query.exec()){
            emit message_change("Reverting changes");
            db->rollback();

            emit task_complete(TaskError("Failed to insert row."));
            return;
        }

        query.bindValue(0, "dataDirectory");
        query.bindValue(1, datasetPath);
        if (!query.exec()){
            emit message_change("Reverting changes");
            db->rollback();

            emit task_complete(TaskError("Failed to insert row."));
            return;
        }
    }

    // =================================================================================================================
    // Load grids
    // =================================================================================================================

    {
        QFile gridFile(QDir(datasetPath).filePath("rectangle_grid_table.csv"));
        gridFile.open(QIODevice::ReadOnly);

        QSqlQuery query(*db);

        gridFile.readLine();
        while (!gridFile.atEnd()){
            QString line(gridFile.readLine().trimmed());

            auto sql = QString("insert into grids (grid_id,vertex0_lng,vertex0_lat,vertex1_lng,vertex1_lat,vertex2_lng,vertex2_lat,vertex3_lng,vertex3_lat) values (%1)").arg(line);
            if (!query.exec(sql)){
                emit message_change("Reverting changes");
                db->rollback();

                emit task_complete(TaskError("Failed to insert grid table."));
                return;
            }
        }
    }

    // =================================================================================================================
    // Load orders
    // =================================================================================================================

    {
        // field & range configs
        QString placeholders{'?'};
        for (int i = 1; i < fields.length(); i++) {
            placeholders.append(",?");
        }

        // find all data files
        QStringList files = QDir(datasetPath).entryList(QStringList() << "order_*", QDir::Files);

        // filter files by range
        QStringList loadFiles{};
        foreach (QString filename, files) {
            auto fileDate = filename.mid(6, 8).toInt();
            if (!(spanLower <= fileDate && fileDate <= spanHigher)) {
                continue;
            }

            filename = QDir(datasetPath).filePath(filename);
            loadFiles.append(filename);
        }

        // start insert data
        qint16 totalCount(loadFiles.length());
        qint16 currentCount(0);

        QSqlQuery query(*db);
        auto sql = QString("insert into orders (%1) values (%2)").arg(fields.join(',')).arg(placeholders);
        query.prepare(sql);

        foreach (QString filename, loadFiles) {
            qreal percentage = qreal(currentCount) / totalCount * 100;
            emit progress_change(percentage);
            emit message_change(QString("Loading dataset ").append(QString::number(round(percentage))).append("%"));

            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly)) {
                emit show_progress(false);
                emit message_change("Reverting changes");
                db->rollback();

                emit task_complete(TaskError(QString("Unable to open file - ").append(file.fileName())));
                return;
            }

            QStringList dataList;
            file.readLine();
            while (!file.atEnd()) {
                if (cancelled) {
                    emit show_progress(false);
                    emit message_change("Reverting changes");
                    db->rollback();

                    emit task_complete(TaskError("Task cancelled."));
                    return;
                }

                auto line = file.readLine().trimmed();
                auto order = line.split(',');

                query.bindValue(0, order[0].trimmed());
                int j = 1;
                for (int i = 1; i < 8; i++) {
                    if (fields.contains(allFields[i])) {
                        query.bindValue(j, order[i].toDouble());
                        j++;
                    }
                }

                if (!query.exec()) {
                    emit show_progress(false);
                    emit message_change("Reverting changes");
                    db->rollback();

                    emit task_complete(TaskError("Failed to insert row."));
                    return;
                }
            }

            currentCount++;
        }

        // create index
        auto sql_create_index = "create index orders_index on orders (departure_time,end_time,orig_lng,orig_lat,dest_lng,dest_lat,fee)";
        emit show_progress(false);
        emit message_change("Creating index");
        if (!query.exec(sql_create_index)) {
            emit show_progress(false);
            emit message_change("Reverting changes");
            db->rollback();

            emit task_complete(TaskError("Failed to create index."));
            return;
        }
    };

    // =================================================================================================================
    // Commit changes
    // =================================================================================================================

    emit show_progress(false);
    emit message_change("Committing transaction");
    if (!db->commit()) {
        emit task_complete(TaskError("Failed to commit transaction."));
        return;
    }

    emit task_complete({});
}
