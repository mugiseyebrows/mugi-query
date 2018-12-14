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

    DisplayFormat::Locale dateTimeLocale() const;
    DisplayFormat::Locale realLocale() const;

    QString dateTime() const;
    QString date() const;
    QString time() const;

private slots:
    void on_dateSpecial_clicked();
    void on_dateCurrentLocale_clicked();
    void on_dateCLocale_clicked();
protected slots:
    void accept() override;

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
