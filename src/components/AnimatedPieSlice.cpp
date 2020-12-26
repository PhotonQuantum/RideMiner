//
// Created by lightquantum on 2020/12/25.
//

#include <cmath>
#include "AnimatedPieSlice.h"

void AnimatedPieSlice::showHighlight(bool isVisible) {
    setLabelVisible(isVisible);
    setExploded(isVisible);
}

AnimatedPieSlice::AnimatedPieSlice(const QString& label, qreal value, qreal percentage) {
    setValue(value);
    setLabel(QString("%1 %2 - %3%").arg(label).arg(value).arg(round(percentage)));
    connect(this, &AnimatedPieSlice::hovered, this, &AnimatedPieSlice::showHighlight);
}
