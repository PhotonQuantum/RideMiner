//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_INITDATABASE_H
#define PSPPROJ_INITDATABASE_H

#include "../Task.h"

class TaskInitDatabase final: public Task {
public:
    using Task::Task;

    QString name() override {
        return "Initialize Database";
    }

protected:
    void run() override;
};

#endif //PSPPROJ_INITDATABASE_H
