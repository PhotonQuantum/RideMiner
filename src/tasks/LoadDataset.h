//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_LOADDATASET_H
#define PSPPROJ_LOADDATASET_H

#include "../Task.h"

class TaskLoadDataset final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "Load Dataset";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_LOADDATASET_H
