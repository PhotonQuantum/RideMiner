//
// Created by lightquantum on 2020/12/25.
//

#include "../Task.h"

#ifndef PSPPROJ_CHARTDEMAND_H
#define PSPPROJ_CHARTDEMAND_H

class TaskChartDemand final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "DemandSense";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_CHARTDEMAND_H
