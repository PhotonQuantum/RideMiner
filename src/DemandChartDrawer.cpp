//
// Created by lightquantum on 2020/12/25.
//

#include "DemandChartDrawer.h"
#include "utils.h"
#include "components/AnimatedPieSlice.h"

QChart *drawLineChart(QList<QLongIntPair> &dataSeries) {
    QLineSeries *series;
    if (dataSeries.length() < 30) {
        series = new QSplineSeries;
    } else {
        series = new QLineSeries;
    }

    for (auto &[x, y]: dataSeries) {
        series->append(x, y);
    }
    auto *chart = new QChart;
    auto *axisX = new QDateTimeAxis;
    auto *axisY = new QValueAxis;
    axisY->setTitleText("Orders");
    axisX->setFormat("MM-dd hh:mm");
    axisX->setLabelsAngle(-90);
    axisX->setTickCount(10);

    axisY->setLabelFormat("%i");

    chart->setAnimationOptions(QChart::AllAnimations);
    chart->addSeries(series);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    axisY->setMin(0);
    if (dataSeries.length() < 30) {
        axisY->setMax(axisY->max() * 1.2 + 1);
    }
    axisY->setMax(roundAxis(axisY->max()));

    chart->legend()->hide();

    return chart;
}

QChart *drawPieChart(QList<QLongIntPair> &dataSeries) {
    qint64 traffic[24]{0};
    qint64 allTraffic = 0;

    for (auto[x, y]: dataSeries) {
        auto time = QDateTime::fromMSecsSinceEpoch(x).time();
        traffic[time.hour()] += y;
        allTraffic += y;
    }

    auto *series = new QPieSeries;
    for (int i = 0; i < 24; i++) {
        QString label;
        label = QString::number((i==0)?12:(i)%12);
        label.append((i >= 12) ? "PM" : "AM");

        auto *slice = new AnimatedPieSlice(label, traffic[i], qreal(traffic[i]) / allTraffic * 100);
        series->append(slice);
    }

    auto *chart = new QChart;

    auto font = chart->font();
    font.setBold(true);
    font.setPixelSize(24);
    chart->setTitleFont(font);

    chart->setTitle("Orders per hour");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->addSeries(series);
    chart->legend()->hide();

    return chart;
}
