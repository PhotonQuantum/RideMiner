//
// Created by lightquantum on 2020/12/23.
//

#include <QEventLoop>
#include <QTimer>
#include "Scheduler.h"

void Scheduler::run() {
    QEventLoop loop;
    _run();
    loop.exec();
    if (running) {
        Task *task = currentTask;
        task->cancel();
        task->quit();
        task->wait();
    }
}

void Scheduler::_run() {
    if (running) {
        return;
    }

    muTask.lockForRead();
    if (taskQueue.isEmpty()){
        muTask.unlock();
        return;
    }
    muTask.unlock();

    running = true;
    muTask.lockForWrite();
    Task *task = taskQueue.dequeue();
    muTask.unlock();

    connect(task, &Task::show_progress, this, &Scheduler::show_progress);
    connect(task, &Task::progress_change, this, &Scheduler::progress_change);
    connect(task, &Task::message_change, this, &Scheduler::_message_change);
    connect(task, &Task::task_complete, this, &Scheduler::on_complete);

    currentTask.store(task);

    emit progress_change(0);
    emit status_change(true);
    task->start();
}

void Scheduler::schedule(Task *task) {
    connect(task, &Task::finished, task, &QObject::deleteLater);

    muTask.lockForWrite();
    taskQueue.append(task);
    muTask.unlock();

    QMetaObject::invokeMethod(this, &Scheduler::_run);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
AsyncFuture::Observable<TaskResult> Scheduler::promise(Task *task) {
    auto future = AsyncFuture::observe(task, &Task::task_complete);
#else
QFuture<TaskResult> Scheduler::promise(Task *task) {
    auto future = AsyncFuture::observe(task, &Task::task_complete).future();
#endif
    schedule(task);
    return future;
}

void Scheduler::on_complete(TaskResult result) {
    running = false;
    currentTask.store(nullptr);
    emit status_change(false);
    if (auto error = std::get_if<TaskError>(&result)) {
        emit progress_change(0);
        emit message_change(dynamic_cast<Task *>(QObject::sender())->name().append(" failed: ").append(error->reason));
    } else {
        emit progress_change(100);
        emit message_change("Idle");
        QMetaObject::invokeMethod(this, &Scheduler::_run);
    }
}

void Scheduler::cancel(bool blocking) {
    if (running) {
        Task *task = currentTask;
        task->cancel();
        task->quit();
        if (blocking) {
            task->wait();
        }
    }
}

void Scheduler::_message_change(const QString &msg) {
    QString message(dynamic_cast<Task *>(QObject::sender())->name());
    message.append(": ").append(msg);
    if (taskQueue.length() > 0) {
        message.append(" (").append(QString::number(taskQueue.length())).append(" pending)");
    }
    emit message_change(message);
}

