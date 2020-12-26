//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_DATABASE_H
#define PSPPROJ_DATABASE_H
#include <QSqlDatabase>
#include <QMutex>

class Database {
private:
    static QMutex muDB;
public:
    std::optional<QSqlDatabase> get();

    Database();
    ~Database();

    static bool createConnection();

    static void closeConnection();
};

#endif //PSPPROJ_DATABASE_H
