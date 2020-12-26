//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_SCHEDULER_H
#define PSPPROJ_SCHEDULER_H

#include <QThread>
#include <QQueue>
#include <QReadWriteLock>
#include <QVariant>
#include <QFuture>
#include "Task.h"
#include "asyncfuture.h"

class Scheduler : public QThread {
Q_OBJECT
signals:

    void show_progress(bool isVisible);

    void progress_change(qreal percentage);

    void message_change(const QString &msg);

    void status_change(bool isBusy);

private slots:

    void on_complete(TaskResult result);

    void _message_change(const QString &msg);

private:
    QQueue<Task *> taskQueue;
    QReadWriteLock muTask;
    std::atomic<bool> running;
    std::atomic<Task *> currentTask;
    void _run();
protected:
    void run() override;

public:
    explicit Scheduler(QObject *parent = nullptr) : QThread(parent), running(false), currentTask(nullptr) {};

    void schedule(Task *task);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    AsyncFuture::Observable<TaskResult> promise(Task *task);
#else
    QFuture<TaskResult> promise(Task *task);
#endif

    void cancel(bool blocking=false);
};

#endif //PSPPROJ_SCHEDULER_H
