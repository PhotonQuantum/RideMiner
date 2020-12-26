//
// Created by lightquantum on 2020/12/23.
//

#include "dummy_task.h"

void DummyTask::run() {
    for (int i=0; i<100; i++){
        if (cancelled) {
            emit task_complete(TaskError(QString("Task cancelled.")));
            return;
        }
        emit message_change(QString("Processing %0").arg(i));
        emit progress_change(i);
        msleep(50);
    }
    emit task_complete(QString("success-").append(args.first().toString()));
}
