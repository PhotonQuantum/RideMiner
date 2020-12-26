//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_UTILS_H
#define PSPPROJ_UTILS_H

#include <QString>
#include <QMessageBox>
#include <QtCore>
#include "types.h"

QMessageBox *messageBox(const QString &title, const QString &msg, const QMessageBox::Icon &icon = QMessageBox::NoIcon,
                        QMessageBox::StandardButtons buttons = QMessageBox::Ok, bool modal = false);

DatabasePath getDatabasePath(bool allowCreation = true);

std::optional<QString> getPersistKV(const QString &key);

bool setPersistKV(const QString &key, const QString &value);

std::optional<QIntPair> getConfigRange();

std::optional<QList<Grid>> getGrids();

void removeDatabase();

qint32 roundAxis(qint32 axis);


template<typename T>
void mustItem(QList<T> &list, qint32 idx) {
    while (list.length() - 1 < idx) {
        list.append(0);
    }
}

#endif //PSPPROJ_UTILS_H
