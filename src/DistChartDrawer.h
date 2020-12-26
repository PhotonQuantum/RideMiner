//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_DISTCHARTDRAWER_H
#define PSPPROJ_DISTCHARTDRAWER_H

#include "types.h"
#include <QList>
#include <QtCharts>

QChart *drawTravelTime(QList<qint32> &);

QChart *drawOrderFee(QList<qint32> &);

QChart *drawRevenue(QList<qreal> &);

#endif //PSPPROJ_DISTCHARTDRAWER_H
