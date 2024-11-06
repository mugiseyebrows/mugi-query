#include "settingsdirectorydialog.h"
#include "ui_settingsdirectorydialog.h"

#include "settings.h"
#include <QDir>

SettingsDirectoryDialog::SettingsDirectoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDirectoryDialog)
{
    ui->setupUi(this);
    ui->dir->setMode(LineSelect::ModeDir);
    ui->dir->setText(Settings::instance()->homePath());
}

SettingsDirectoryDialog::~SettingsDirectoryDialog()
{
    delete ui;
}

void SettingsDirectoryDialog::accept()
{
    QString path = ui->dir->text();
#if 0
    if (path.isEmpty() || !QDir(path).exists()) {
        return;
    }
#endif
    Settings::instance()->setHomePath(path);
    QDialog::accept();
}


