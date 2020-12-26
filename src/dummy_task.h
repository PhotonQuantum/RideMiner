//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_DUMMY_TASK_H
#define PSPPROJ_DUMMY_TASK_H

#include "Task.h"

class DummyTask final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "dummy_task";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_DUMMY_TASK_H
