//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_ANIMATEDPIESLICE_H
#define PSPPROJ_ANIMATEDPIESLICE_H

#include <QPieSlice>

QT_CHARTS_BEGIN_NAMESPACE

    class QAbstractSeries;

    class QPieSlice;

QT_CHARTS_END_NAMESPACE

using namespace QtCharts;

class AnimatedPieSlice : public QPieSlice {
Q_OBJECT

public:
    AnimatedPieSlice(const QString& label, qreal value, qreal percentage);

    ~AnimatedPieSlice() override = default;

public slots:

    void showHighlight(bool isVisible);
};

#endif //PSPPROJ_ANIMATEDPIESLICE_H
