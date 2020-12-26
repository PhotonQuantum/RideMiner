//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_TASK_H
#define PSPPROJ_TASK_H

#include <QThread>
#include <QVariant>
#include <utility>
#include <QMetaType>

struct TaskError {
    QString reason;
    explicit TaskError(QString reason): reason(std::move(reason)){};
};

using TaskResult = std::variant<QVariant, TaskError>;

Q_DECLARE_METATYPE(TaskResult);

class Task : public QThread {
Q_OBJECT
signals:

    void show_progress(bool isVisible);

    void progress_change(qreal percentage);

    void message_change(const QString &msg);

    void task_complete(TaskResult result);

public:
    explicit Task(QVariantList args, QObject *parent = nullptr) : QThread(parent), args(std::move(args)), cancelled(false) {};
    explicit Task(QObject *parent = nullptr) : QThread(parent), args(QVariantList{}), cancelled(false) {};

    virtual QString name() = 0;

    void cancel() {
        cancelled = true;
    }

protected:
    QVariantList args;
    std::atomic<bool> cancelled;
    ~Task() override = default;
};

#endif //PSPPROJ_TASK_H
