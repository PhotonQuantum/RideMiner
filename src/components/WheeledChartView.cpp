//
// Created by lightquantum on 2020/12/25.
//

#include "WheeledChartView.h"
#include <QValueAxis>

void WheeledChartView::wheelEvent(QWheelEvent *event) {
    auto *chart = this->chart();
    if (!chart) {
        return;
    }
    if (event->angleDelta().y() > 0){
        chart->zoomIn();
    } else {
        chart->zoomOut();
    }
    QGraphicsView::wheelEvent(event);
}

void WheeledChartView::mouseDoubleClickEvent(QMouseEvent *event) {
    auto *chart = this->chart();
    if (!chart) {
        return;
    }
    chart->zoomReset();
    QGraphicsView::mouseDoubleClickEvent(event);
}

void WheeledChartView::showEvent(QShowEvent *event) {
    emit ShowSignal(event);
    QGraphicsView::showEvent(event);
}

bool WheeledChartView::eventFilter(QObject *watched, QEvent *event) {
    if(watched == rubberBand && event->type() == QEvent::Resize){
        QPointF fp = chart()->mapToValue(rubberBand->geometry().topLeft());
        QPointF tp = chart()->mapToValue(rubberBand->geometry().bottomRight());
        emit rubberBandChanged(fp, tp);
    }
    return QChartView::eventFilter(watched, event);
}

void WheeledChartView::setRubberBand(const QChartView::RubberBands &rubberBands) {
    QChartView::setRubberBand(rubberBands);
    rubberBand = findChild<QRubberBand *>();
    rubberBand->installEventFilter(this);
}

void WheeledChartView::limitRubberBand() {
    auto topLeft = rubberBand->geometry().topLeft();
    auto bottomRight = rubberBand->geometry().bottomRight();
    auto bottom = chart()->mapToValue(bottomRight).y();
    if (bottom < 0){
        bottomRight.setY(chart()->mapToPosition(QPointF{0, 0}).y());
        rubberBand->setGeometry(QRect{topLeft, bottomRight});
    }
}

WheeledChartView::WheeledChartView(QWidget *parent) : QChartView(parent), rubberBand(nullptr) {
    connect(this, &WheeledChartView::rubberBandChanged, this, &WheeledChartView::limitRubberBand);
}

WheeledChartView::WheeledChartView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    connect(this, &WheeledChartView::rubberBandChanged, this, &WheeledChartView::limitRubberBand);
}
