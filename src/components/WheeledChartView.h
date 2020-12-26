//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_WHEELEDCHARTVIEW_H
#define PSPPROJ_WHEELEDCHARTVIEW_H

#include <QChartView>
#include <QRubberBand>

using namespace QtCharts;

class WheeledChartView: public QChartView{
    Q_OBJECT
signals:
    void ShowSignal(QShowEvent *event);
    void rubberBandChanged(QPointF fp, QPointF tp);
public:
    explicit WheeledChartView(QWidget *parent = nullptr);
    explicit WheeledChartView(QChart *chart, QWidget *parent = nullptr);
    bool eventFilter(QObject *watched, QEvent *event) override;
    void setRubberBand(const RubberBands &rubberBands);
    QRubberBand *rubberBand;
protected:
    void wheelEvent(QWheelEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void showEvent(QShowEvent *event) override;
private:
    void limitRubberBand();
};

#endif //PSPPROJ_WHEELEDCHARTVIEW_H
