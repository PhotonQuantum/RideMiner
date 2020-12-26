//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_DEMANDCHARTDRAWER_H
#define PSPPROJ_DEMANDCHARTDRAWER_H

#include "types.h"
#include <QList>
#include <QtCharts>

QChart* drawLineChart(QList<QLongIntPair>&);

QChart* drawPieChart(QList<QLongIntPair>&);

#endif //PSPPROJ_DEMANDCHARTDRAWER_H