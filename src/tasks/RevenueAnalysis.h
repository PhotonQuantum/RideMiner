//
// Created by lightquantum on 2020/12/26.
//

#ifndef PSPPROJ_REVENUEANALYSIS_H
#define PSPPROJ_REVENUEANALYSIS_H

#include "../Task.h"

class TaskRevenueAnalysis final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "DistInsight";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_REVENUEANALYSIS_H
