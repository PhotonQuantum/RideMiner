#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dummy_task.h"
#include "tasks/InitDatabase.h"
#include "tasks/LoadDataset.h"
#include "tasks/ChartDemand.h"
#include "tasks/BasicStats.h"
#include "tasks/TravelTimeAnalysis.h"
#include "tasks/OrderFeeAnalysis.h"
#include "tasks/RevenueAnalysis.h"
#include "tasks/ETAPrediction.h"
#include "DemandChartDrawer.h"
#include "DistChartDrawer.h"
#include "utils.h"
#include "Database.h"
#include "Config.h"
#include <QDateTime>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        pbStatus(new QProgressBar(this)),
        lblStatus(new QLabel(this)),
        btnCancelTask(new QPushButton("x", this)),
        spinnerStatus(new WaitingSpinnerWidget(this, false, false)),
        spcStatus(new QWidget(this)) {
    ui->setupUi(this);
    ui->widgetMain->setEnabled(false);
    SetupStatusBar();

    ui->chartDemand->setRubberBand(QChartView::RectangleRubberBand);
    ui->chartInspector->setRubberBand(QChartView::RectangleRubberBand);

    connect(&scheduler, &Scheduler::show_progress, this, &MainWindow::ShowProgress);
    connect(&scheduler, &Scheduler::progress_change, this, &MainWindow::UpdateProgress);
    connect(&scheduler, &Scheduler::message_change, this, &MainWindow::UpdateMessage);
    connect(&scheduler, &Scheduler::status_change, this, &MainWindow::SetBusyStatus);

    connect(btnCancelTask, &QPushButton::clicked, this, &MainWindow::btnCancelTask_clicked);
    connect(ui->btnQuery, &QPushButton::clicked, this, &MainWindow::btnQuery_clicked);
    connect(ui->btnPredict, &QPushButton::clicked, this, &MainWindow::btnPredict_clicked);

    connect(ui->chartInspector, &WheeledChartView::ShowSignal, [=]() {
        static bool hasDrawn = false;
        if (!hasDrawn) {
            hasDrawn = true;
            QMetaObject::invokeMethod(this, &MainWindow::DrawInspector);
        }
    });

    connect(ui->optTravel, &QRadioButton::clicked, this, &MainWindow::DrawInspector);
    connect(ui->optFee, &QRadioButton::clicked, this, &MainWindow::DrawInspector);
    connect(ui->optRevenue, &QRadioButton::clicked, this, &MainWindow::DrawInspector);
    connect(ui->optGlobal, &QRadioButton::clicked, [=]() {
        ui->cbGrid->setEnabled(false);
    });
    connect(ui->optGrid, &QRadioButton::clicked, [=]() {
        ui->cbGrid->setEnabled(true);
    });
    connect(ui->optLineChart, &QRadioButton::clicked, [=]() {
        ui->lblStep->setEnabled(true);
        ui->widgetStep->setEnabled(true);
    });
    connect(ui->optPieChart, &QRadioButton::clicked, [=]() {
        ui->lblStep->setEnabled(false);
        ui->widgetStep->setEnabled(false);
    });

    connect(ui->mnuInvalidateCache, &QAction::triggered, DBReset);
    connect(ui->mnuExit, &QAction::triggered, []() {
        QApplication::exit(0);
    });
}

void MainWindow::SetupStatusBar() {
    pbStatus->setMaximumHeight(10);
    pbStatus->setTextVisible(false);
    pbStatus->setMinimum(0);
    pbStatus->setMaximum(100);

    spinnerStatus->setRoundness(10);
    spinnerStatus->setInnerRadius(4);
    spinnerStatus->setLineLength(4);
    spinnerStatus->setNumberOfLines(10);

    btnCancelTask->setMaximumHeight(15);
    btnCancelTask->setMaximumWidth(15);

    spcStatus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    ui->statusbar->addPermanentWidget(spcStatus, 8);
    ui->statusbar->addPermanentWidget(spinnerStatus, 1);
    ui->statusbar->addPermanentWidget(lblStatus);
    ui->statusbar->addPermanentWidget(pbStatus, 4);
    ui->statusbar->addPermanentWidget(btnCancelTask, 1);
}

void MainWindow::UpdateProgress(qreal percent) {
    pbStatus->setValue(percent);
}

void MainWindow::UpdateMessage(const QString &msg) {
    lblStatus->setText(msg);
    lblStatus->repaint();
}

void MainWindow::DBReset() {
    auto msgBox = messageBox("Invalidate Caches", "The caches will be invalidated and rebuilt on the next startup.",
                             QMessageBox::Warning, QMessageBox::Ok | QMessageBox::Cancel, true);
    msgBox->setButtonText(QMessageBox::Ok, tr("Invalidate and Restart"));

    connect(msgBox, &QMessageBox::accepted, [=]() {
        if (Config::getInstance().get("isPersist").value().toBool()) {
            removeDatabase();
        }

        QApplication::exit(114514);
    });
}

void MainWindow::InitializeBasicInfo() {
    auto dataDirectory = getPersistKV("dataDirectory");
    if (!dataDirectory){
        messageBox("Error", "Unable to read info: dataDirectory\nYou may want to invalidate caches & restart.",
                   QMessageBox::Critical, QMessageBox::Ok, true);
        return;
    }
    ui->lblSource->setText(dataDirectory.value());

    auto range = getConfigRange();
    if (!dataDirectory){
        messageBox("Error", "Unable to read info: range\nYou may want to invalidate caches & restart.",
                   QMessageBox::Critical, QMessageBox::Ok, true);
        return;
    }
    auto [spanLower, spanUpper] = getConfigRange().value();
    ui->lblFrom->setText(QString::number(spanLower));
    ui->lblTo->setText(QString::number(spanUpper));

    auto *task = new TaskBasicStats;
    scheduler.promise(task).then([=](const TaskResult &result) {
        if (auto data = std::get_if<QVariant>(&result)) {
            auto[orders, revenue] = data->value<QLongIntRealPair>();
            ui->lblOrders->setText(QString::number(orders));
            ui->lblRevenue->setText((revenue == -1) ? "N/A" : QString::number(revenue, 'f', 2));
        }
    });
}

void MainWindow::InitializeQueryDefault() {
    auto range = getConfigRange();
    if (!range) {
        messageBox("Error", "Unable to read config: range\nYou may want to invalidate caches & restart.",
                   QMessageBox::Critical, QMessageBox::Ok, true);
        return;
    }
    auto[spanLower, spanHigher] = range.value();

    auto grids = getGrids();
    if (!grids) {
        messageBox("Error", "Unable to read config: grids\nYou may want to invalidate caches & restart.",
                   QMessageBox::Critical, QMessageBox::Ok, true);
        return;
    }
    auto gridCount = grids.value().length();

    ui->cbGrid->clear();
    for (int i = 0; i < gridCount; i++) {
        ui->cbGrid->addItem(QString::number(i));
    }

    auto dateLower = QDate::fromString(QString::number(spanLower), "yyyyMMdd");
    auto dateHigher = QDate::fromString(QString::number(spanHigher), "yyyyMMdd");

    ui->dtFrom->setDate(dateLower);
    ui->dtTo->setDate(dateHigher.addDays(1));

    auto rawFields = getPersistKV("fields");
    if (!rawFields){
        messageBox("Error", "Unable to read config: fields\nYou may want to invalidate caches & restart.",
                   QMessageBox::Critical, QMessageBox::Ok, true);
        return;
    }
    auto fields = rawFields->split(',');
    if (!(fields.contains("fee"))){
        ui->optFee->setEnabled(false);
        ui->optRevenue->setEnabled(false);
    }

    ui->widgetMain->setEnabled(true);
};

void MainWindow::DBPrepare() {
    if (!Database::createConnection()) {
        Panic("Fatal", "Unable to create connection to database.");
    }

    auto *task = new TaskInitDatabase;

    scheduler.promise(task).then([=](const TaskResult &result) {
        if (auto error = std::get_if<TaskError>(&result)) {
            Database::closeConnection();
            if (Config::getInstance().get("isPersist").value().toBool()) {
                removeDatabase();
            }
            Panic("Fatal", QString("Unable to prepare database: ").append(error->reason));
        }
    });

    auto dataDirectory = Config::getInstance().get("dataDirectory");
    if (dataDirectory) {
        auto *task2 = new TaskLoadDataset({dataDirectory->toString()});

        scheduler.promise(task2).then([=](const TaskResult &result) {
            if (auto error = std::get_if<TaskError>(&result)) {
                Database::closeConnection();
                if (Config::getInstance().get("isPersist").value().toBool()) {
                    removeDatabase();
                }
                Panic("Fatal", QString("Unable to load dataset: ").append(error->reason));
                return;
            }

            QMetaObject::invokeMethod(this, &MainWindow::InitializeBasicInfo);
            QMetaObject::invokeMethod(this, &MainWindow::InitializeQueryDefault);
        });
    } else {
        QMetaObject::invokeMethod(this, &MainWindow::InitializeBasicInfo);
        QMetaObject::invokeMethod(this, &MainWindow::InitializeQueryDefault);
    }
}

MainWindow::~MainWindow() {
    scheduler.quit();
    scheduler.wait();
    delete ui;
}

void MainWindow::btnCancelTask_clicked() {
    scheduler.cancel();
}

void MainWindow::SetBusyStatus(bool isBusy) {
    if (isBusy) {
        spinnerStatus->start();
    } else {
        spinnerStatus->stop();
    }
    pbStatus->setVisible(isBusy);
    btnCancelTask->setVisible(isBusy);
}

void MainWindow::Panic(const QString &title, const QString &msg) {
    auto msgBox = messageBox(title, msg, QMessageBox::Critical, QMessageBox::Cancel, true);
    connect(msgBox, &QMessageBox::buttonClicked, [=]() {
        QApplication::exit(1);
    });
}

void MainWindow::Show() {
    scheduler.start();

    if (!Config::getInstance().get("isPersist").value().toBool()) {
        ui->lblMode->setText("Memory");
        ui->lblDBPath->setText("In Memory");
    } else {
        ui->lblMode->setText("Persistent");
        auto[_, filePath, _b] = getDatabasePath();
        ui->lblDBPath->setText(filePath);
    }

    DBPrepare();
}

void MainWindow::ShowProgress(bool isVisible) {
    pbStatus->setVisible(isVisible);
}

void MainWindow::btnQuery_clicked() {
    qint32 step;
    if (ui->optMinutely->isChecked()) {
        step = 60 * ui->edtStep->text().toInt();
    } else if (ui->optHourly->isChecked()) {
        step = 3600 * ui->edtStep->text().toInt();
    } else {
        step = 86400 * ui->edtStep->text().toInt();
    }

    if (ui->optPieChart->isChecked()) {
        ui->dtFrom->setTime(QTime(0, 0));
        ui->dtTo->setTime(QTime(0, 0));
    }

    auto *task = new TaskChartDemand({ui->dtFrom->dateTime().toMSecsSinceEpoch() / 1000,
                                      ui->dtTo->dateTime().toMSecsSinceEpoch() / 1000,
                                      ui->optDepart->isChecked(),
                                      ui->optGlobal->isChecked() ? -1 : ui->cbGrid->currentIndex(),
                                      ui->optLineChart->isChecked() ? step : 3600});

    scheduler.promise(task).then([=](const TaskResult &result) {
        if (auto _dataSeries = std::get_if<QVariant>(&result)) {
            auto dataSeries = _dataSeries->value<QList<QLongIntPair>>();

            QChart *chart;
            if (ui->optLineChart->isChecked()) {
                chart = drawLineChart(dataSeries);
            } else {
                chart = drawPieChart(dataSeries);
            }
            ui->chartDemand->setChart(chart);
        }
    });
}

void MainWindow::DrawInspector() {
    if (ui->optTravel->isChecked()) {
        auto *task = new TaskTravelTimeAnalysis;
        scheduler.promise(task).then([=](const TaskResult &result) {
            if (auto data = std::get_if<QVariant>(&result)) {
                auto dataSeries = data->value<QList<qint32>>();

                QChart *chart;
                chart = drawTravelTime(dataSeries);
                ui->chartInspector->setChart(chart);
            };
        });
    } else if (ui->optFee->isChecked()) {
        auto *task = new TaskOrderFeeAnalysis;
        scheduler.promise(task).then([=](const TaskResult &result) {
            if (auto data = std::get_if<QVariant>(&result)) {
                auto dataSeries = data->value<QList<qint32>>();

                QChart *chart;
                chart = drawOrderFee(dataSeries);
                ui->chartInspector->setChart(chart);
            };
        });
    } else {
        auto *task = new TaskRevenueAnalysis;
        scheduler.promise(task).then([=](const TaskResult &result) {
            if (auto data = std::get_if<QVariant>(&result)) {
                auto dataSeries = data->value<QList<qreal>>();

                QChart *chart;
                chart = drawRevenue(dataSeries);
                ui->chartInspector->setChart(chart);
            };
        });
    }
}

void MainWindow::btnPredict_clicked() {
    auto departTime = ui->edtDepart->time().msecsSinceStartOfDay()/1000;
    auto departCoord = QPointF(ui->edtLngDepart->text().toDouble(),
                               ui->edtLatDepart->text().toDouble());
    auto arrivalCoord = QPointF(ui->edtLngArrival->text().toDouble(),
                               ui->edtLatArrival->text().toDouble());
    auto *task = new TaskETAPrediction({departTime, departCoord, arrivalCoord});
    scheduler.promise(task).then([=](const TaskResult &result) {
        if (auto data = std::get_if<QVariant>(&result)) {
            auto eta = data->toInt();

            if (eta == -1){
                ui->lblPrediction->setText(QString("Insufficient data."));
            } else {
                ui->lblPrediction->setText(QString("Estimated Time of Arrival: %1 minutes").arg(eta / 60));
            }
        };
    });
}
