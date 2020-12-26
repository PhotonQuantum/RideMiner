//
// Created by lightquantum on 2020/12/23.
//

#include <QDir>
#include "utils.h"
#include "Config.h"
#include "Database.h"
#include <QSqlQuery>
#include <QAbstractButton>

QMessageBox *messageBox(const QString &title, const QString &msg, const QMessageBox::Icon &icon,
                        QMessageBox::StandardButtons buttons, bool modal) {
    auto *msgBox(new QMessageBox);
    msgBox->setModal(true);
    msgBox->setWindowTitle(title);
    msgBox->setText(msg);
    msgBox->setStandardButtons(buttons);
    msgBox->setIcon(icon);
    msgBox->show();

    QMessageBox::connect(msgBox, &QMessageBox::buttonClicked, msgBox, &QObject::deleteLater);

    return msgBox;
}

std::optional<QString> getPersistKV(const QString &key) {
    Database dbConn;
    auto _db = dbConn.get();
    if (!_db) {
        return {};
    }

    auto db = _db.value();

    QSqlQuery query(db);
    query.prepare("select value from kv where key = ?");
    query.bindValue(0, key);

    if (!query.exec()) {
        return {};
    };

    query.next();
    if (query.at() != 0) {
        return {};
    }

    return query.value(0).toString();
}

bool setPersistKV(const QString &key, const QString &value){
    Database dbConn;
    auto _db = dbConn.get();
    if (!_db) {
        return false;
    }

    auto db = _db.value();

    QSqlQuery query(db);
    query.prepare("insert into kv (key,value) values (?,?)");
    query.bindValue(0, key);
    query.bindValue(1, value);

    if (!query.exec()) {
        return false;
    };

    return true;
}

std::optional<QIntPair> getConfigRange() {
    auto value = getPersistKV("range");
    if (!value) {
        return {};
    }

    auto range = value.value().split(',');
    return QIntPair{range[0].toInt(), range[1].toInt()};
}

std::optional<QList<Grid>> getGrids() {
    Database dbConn;
    auto _db = dbConn.get();
    if (!_db) {
        return {};
    }

    auto db = _db.value();

    QSqlQuery query(db);
    if (!query.exec(
            "select grid_id,vertex0_lng,vertex0_lat,vertex1_lng,vertex1_lat,vertex2_lng,vertex2_lat,vertex3_lng,vertex3_lat from grids order by grid_id")) {
        return {};
    }

    QList<Grid> grids{};

    while (query.next()) {
        auto grid_id = query.value("grid_id").toInt();
        auto vertex0 = Point{query.value("vertex0_lng").toReal(), query.value("vertex0_lat").toReal()};
        auto vertex1 = Point{query.value("vertex1_lng").toReal(), query.value("vertex1_lat").toReal()};
        auto vertex2 = Point{query.value("vertex2_lng").toReal(), query.value("vertex2_lat").toReal()};
        auto vertex3 = Point{query.value("vertex3_lng").toReal(), query.value("vertex3_lat").toReal()};
        grids.append(Grid{grid_id, vertex0, vertex1, vertex2, vertex3});
    }

    return grids;
}

void removeDatabase() {
    auto[_, fileName, hasDB] = getDatabasePath(false);
    if (!hasDB) {
        return;
    }

    if (fileName == ":memory:") {
        return;
    }

    QFile qfile(fileName);
    qfile.remove();
}

DatabasePath getDatabasePath(bool allowCreation) {
    auto isPersist = Config::getInstance().get("isPersist");

    if (!isPersist || !isPersist->toBool()) {
        return {"", ":memory:", true};
    }

#ifdef QT_DEBUG
    QString dir("./");
#else
    QString dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
#endif
    QString file(QDir(dir).filePath("db.sqlite"));

    QFile qfile(file);
    if (!qfile.exists()) {
        if (allowCreation) {
            QDir qdir;
            qdir.mkdir(dir);
            qfile.open(QIODevice::NewOnly);
            return DatabasePath{dir, file, true};
        } else {
            return {dir, file, false};
        }
    } else {
        return {dir, file, true};
    }
}

qint32 roundAxis(qint32 axis) {
    if (axis <= 40) {
        return axis;
    } else if (axis <= 100) {
        return ceil(qreal(axis) / 20) * 20;
    } else {
        return ceil(qreal(axis) / 40) * 40;
    }
}
