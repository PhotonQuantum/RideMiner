//
// Created by lightquantum on 2020/12/26.
//

#ifndef PSPPROJ_ETAPREDICTION_H
#define PSPPROJ_ETAPREDICTION_H

#include "../Task.h"
#include <QPointF>

struct TravelPoint{
    qint32 departTime{};
    QPointF departCoord;
    QPointF arrivalCoord;
};

class TaskETAPrediction final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "intelliPredict";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_ETAPREDICTION_H
