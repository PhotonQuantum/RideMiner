#include "loaderdialog.h"
#include "ui_loaderdialog.h"
#include "Config.h"
#include "utils.h"
#include "types.h"
#include <QDir>
#include <QFileDialog>

LoaderDialog::LoaderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoaderDialog),
    chkFields()
{
    ui->setupUi(this);
    setupUi();

    connect(ui->chkPersist, &QCheckBox::stateChanged, this, &LoaderDialog::chkPersist_changed);
    connect(ui->chkPersist, &QCheckBox::stateChanged, this, &LoaderDialog::checkConfigs);
    connect(ui->btnSelect, &QCheckBox::clicked, this, &LoaderDialog::btnSelect_clicked);
    connect(ui->edtSelect, &QLineEdit::textChanged, this, &LoaderDialog::checkConfigs);
    connect(ui->rngRange, &RangeSlider::lowerValueChanged, this, &LoaderDialog::rngRangeChanged);
    connect(ui->rngRange, &RangeSlider::upperValueChanged, this, &LoaderDialog::rngRangeChanged);
    connect(this, &LoaderDialog::accepted, this, &LoaderDialog::setConfigs);

    QMetaObject::invokeMethod(this, &LoaderDialog::chkPersist_changed);
    QMetaObject::invokeMethod(this, &LoaderDialog::checkConfigs);
}

LoaderDialog::~LoaderDialog()
{
    delete ui;
}

void LoaderDialog::setupUi(){
    static const QStringList fields{"order_id", "departure_time", "end_time", "orig_lng", "orig_lat", "dest_lng", "dest_lat", "fee"};
    static const QStringList requiredFields{"order_id", "departure_time", "end_time", "orig_lng", "orig_lat", "dest_lng", "dest_lat"};

    for (const auto &field: fields){
        auto chkField = new QCheckBox(this);
        chkField->setText(field);
        if (requiredFields.contains(field)){
            chkField->setEnabled(false);
        }
        chkField->setChecked(true);
        ui->layoutFields->addWidget(chkField);

        chkFields.append(chkField);
    }

    ui->rngRange->setMaximum(20161115);
    ui->rngRange->setMinimum(20161101);
}

void LoaderDialog::checkConfigs() {
    bool isValid = _checkConfigs();
    auto &configs = Config::getInstance();

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
    if (auto hasDataset = configs.get("hasDataset"); hasDataset && hasDataset->toBool()) {
        ui->grpRange->setEnabled(isValid);
        ui->grpFields->setEnabled(isValid);
    }
}

bool LoaderDialog::_checkConfigs() {
    auto &configs = Config::getInstance();
    if (auto hasDataset = configs.get("hasDataset"); hasDataset && !hasDataset->toBool()) {
        return true;
    }

    auto path = ui->edtSelect->text();
    QDir dir(path);
    if (!dir.exists() || !dir.exists("rectangle_grid_table.csv")){
        return false;
    }

    if (dir.entryList(QStringList{"order_*.csv"}, QDir::Files).length() == 0){
        return false;
    }

    return true;
}

void LoaderDialog::chkPersist_changed() {
    Config::getInstance().set("isPersist", ui->chkPersist->isChecked());

    if (ui->chkPersist->isChecked()){
        auto [_, _2, hasDB] = getDatabasePath(false);
        if (hasDB) {
            setDatasetEnabled(false);
            ui->lblPersist->setText("Persist data found.\nPlease invalidate cache if you want to reload the dataset.");
        } else {
            setDatasetEnabled(true);
            ui->lblPersist->setText("Data will be persisted into your hard disk.\nPerformance regression is expected.");
        }
    } else {
        setDatasetEnabled(true);
        ui->lblPersist->setText("Data will be loaded directly to memory.\nYou need to reload dataset next time.");
    }
}

void LoaderDialog::setDatasetEnabled(bool isEnabled) {
    Config::getInstance().set("hasDataset", isEnabled);
    ui->edtSelect->setEnabled(isEnabled);
    ui->btnSelect->setEnabled(isEnabled);
    ui->grpRange->setEnabled(isEnabled);
    ui->grpFields->setEnabled(isEnabled);
}

void LoaderDialog::setConfigs() {
    auto &configs = Config::getInstance();
    if (auto hasDataset = configs.get("hasDataset"); hasDataset && !hasDataset->toBool()) {
        return;
    }

    QStringList fields{};
    foreach (const auto &chkField, chkFields){
        auto field = chkField->text();
        if (chkField->isChecked()){
            fields.append(field);
        }
    }

    QIntPair range{ui->rngRange->GetLowerValue(), ui->rngRange->GetUpperValue()};
    configs.set("dataDirectory", ui->edtSelect->text());
    configs.set("loadFields", fields);
    configs.set("loadRange", QVariant::fromValue(range));
}

void LoaderDialog::btnSelect_clicked() {
    auto dirPath = QFileDialog::getExistingDirectory(this, "Choose dataset directory");
    if (!dirPath.isEmpty()) {
        ui->edtSelect->setText(dirPath);
    }
}

void LoaderDialog::rngRangeChanged() {
    ui->lblRange->setText(QString("%1 ~ %2").arg(ui->rngRange->GetLowerValue()).arg(ui->rngRange->GetUpperValue()));
}
