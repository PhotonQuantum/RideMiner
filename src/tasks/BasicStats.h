//
// Created by lightquantum on 2020/12/25.
//

#ifndef PSPPROJ_BASICSTATS_H
#define PSPPROJ_BASICSTATS_H

#include "../Task.h"

class TaskBasicStats final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "Statistics";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_BASICSTATS_H
