//
// Created by lightquantum on 2020/12/24.
//

#ifndef PSPPROJ_TYPES_H
#define PSPPROJ_TYPES_H

#include <QtCore>

typedef QPair<qint32,qint32> QIntPair;
typedef QPair<qint64,qint64> QLongIntPair;
typedef QPair<qint64,qreal> QLongIntRealPair;
Q_DECLARE_METATYPE(QIntPair);
Q_DECLARE_METATYPE(QLongIntPair);
Q_DECLARE_METATYPE(QLongIntRealPair);
Q_DECLARE_METATYPE(qint64);
Q_DECLARE_METATYPE(qreal);

struct DatabasePath {
    QString directoryPath;
    QString filePath;
    bool isExists;
};

struct Point{
    qreal longitude;
    qreal latitude;
};

struct Grid{
    qint32 grid_id;
    Point vertex0;
    Point vertex1;
    Point vertex2;
    Point vertex3;
};


#endif //PSPPROJ_TYPES_H
