//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_ORDERFEEANALYSIS_H
#define PSPPROJ_ORDERFEEANALYSIS_H

#include "../Task.h"

class TaskOrderFeeAnalysis final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "DistInsight";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_ORDERFEEANALYSIS_H
