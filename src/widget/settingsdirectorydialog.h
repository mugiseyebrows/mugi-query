#ifndef SETTINGSDIRECTORYDIALOG_H
#define SETTINGSDIRECTORYDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDirectoryDialog;
}

class SettingsDirectoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDirectoryDialog(QWidget *parent = nullptr);
    ~SettingsDirectoryDialog();

private:
    Ui::SettingsDirectoryDialog *ui;

    // QDialog interface
public slots:
    void accept();

};

#endif // SETTINGSDIRECTORYDIALOG_H
