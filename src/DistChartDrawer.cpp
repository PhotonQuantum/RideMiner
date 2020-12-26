//
// Created by lightquantum on 2020/12/25.
//

#include "components/AnimatedPieSlice.h"
#include "DistChartDrawer.h"
#include "utils.h"

QChart *drawTravelTime(QList<qint32> &dataSeries) {
    auto *series = new QBarSeries;
    auto *set = new QBarSet("");
    QStringList cats{};

    for (int i = 0; i < qMin(dataSeries.length(), 30); i++) {
        cats.append(QString("%1~%2").arg(i * 5).arg((i + 1) * 5 - 1));
        auto x = dataSeries[i];
        set->append(x);
    }

    series->append(set);

    auto *chart = new QChart;
    auto *axisX = new QBarCategoryAxis;
    auto *axisY = new QValueAxis;
    axisX->setTitleText("Duration (minutes)");
    axisX->append(cats);
    axisX->setLabelsAngle(-90);

    axisY->setTitleText("Orders");
    axisY->setLabelFormat("%i");

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(series);
    chart->legend()->hide();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    axisY->setMax(roundAxis(axisY->max()));

    return chart;
}
QChart *drawOrderFee(QList<qint32> &dataSeries) {
    auto *series = new QBarSeries;
    auto *set = new QBarSet("");
    QStringList cats{};

    for (int i = 0; i < qMin(dataSeries.length(), 30); i++) {
        cats.append(QString("%1~%2").arg(i * 2).arg((i + 1) * 2 - 1));
        auto x = dataSeries[i];
        set->append(x);
    }

    series->append(set);

    auto *chart = new QChart;
    auto *axisX = new QBarCategoryAxis;
    auto *axisY = new QValueAxis;
    axisX->setTitleText("Fee (¥)");
    axisX->append(cats);
    axisX->setLabelsAngle(-90);

    axisY->setTitleText("Orders");
    axisY->setLabelFormat("%i");

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(series);
    chart->legend()->hide();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    axisY->setMax(roundAxis(axisY->max()));

    return chart;
}

QChart *drawRevenue(QList<qreal> &dataSeries) {
    qint64 allRevenue = 0;

    for (auto revenue: dataSeries) {
        allRevenue += revenue;
    }

    auto *series = new QPieSeries;
    for (int i = 0; i < 24; i++) {
        QString label;
        label = QString::number((i==0)?12:(i)%12);
        label.append((i >= 12) ? "PM" : "AM");

        auto *slice = new AnimatedPieSlice(label, dataSeries[i], qreal(dataSeries[i]) / allRevenue * 100);
        series->append(slice);
    }

    auto *chart = new QChart;

    auto font = chart->font();
    font.setBold(true);
    font.setPixelSize(24);
    chart->setTitleFont(font);

    chart->setTitle("Revenue per hour (¥)");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->addSeries(series);
    chart->legend()->hide();

    return chart;
}
