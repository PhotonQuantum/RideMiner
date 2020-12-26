#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include "Scheduler.h"
#include "QWaitingSpinner.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    QPushButton *btnCancelTask;
    QProgressBar *pbStatus;
    QLabel *lblStatus;
    WaitingSpinnerWidget *spinnerStatus;
    QWidget *spcStatus;

    Scheduler scheduler;

    void SetupStatusBar();

    void DBPrepare();

    static void DBReset();

private slots:

    void ShowProgress(bool isVisible);

    void UpdateProgress(qreal percent);

    void UpdateMessage(const QString &msg);

    void SetBusyStatus(bool isBusy);

    void btnQuery_clicked();

    void btnPredict_clicked();

    void btnCancelTask_clicked();

    static void Panic(const QString &title, const QString &msg);

    void Show();

    void InitializeQueryDefault();

    void InitializeBasicInfo();

    void DrawInspector();
};

#endif // MAINWINDOW_H
