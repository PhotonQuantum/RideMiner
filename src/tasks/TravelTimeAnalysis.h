//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_TRAVELTIMEANALYSIS_H
#define PSPPROJ_TRAVELTIMEANALYSIS_H

#include "../Task.h"

class TaskTravelTimeAnalysis final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "DistInsight";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_TRAVELTIMEANALYSIS_H
