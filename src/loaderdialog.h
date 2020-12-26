#ifndef LOADERDIALOG_H
#define LOADERDIALOG_H

#include <QDialog>
#include <QMap>
#include <QCheckBox>
#include <QPointer>

namespace Ui {
    class LoaderDialog;
}

class LoaderDialog final : public QDialog {
Q_OBJECT

public:
    explicit LoaderDialog(QWidget *parent = nullptr);

    ~LoaderDialog() final;

private:
    Ui::LoaderDialog *ui;

    bool _checkConfigs();

    void setConfigs();

    void setDatasetEnabled(bool isEnabled);

    QList<QPointer<QCheckBox>> chkFields;

private slots:

    void chkPersist_changed();

    void btnSelect_clicked();

    void rngRangeChanged();

    void checkConfigs();

    void setupUi();
};

#endif // LOADERDIALOG_H
