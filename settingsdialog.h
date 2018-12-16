#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "displayformat.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

protected:
    void setDateTimeEnabled(bool state);
    void setRealEnabled(bool state);
private slots:

    void on_dateTimeUseLocale_clicked(bool checked);

    void on_dateTimeUseSpecial_clicked(bool checked);

    void on_realUseLocale_clicked(bool checked);

protected slots:
    void accept() override;

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
