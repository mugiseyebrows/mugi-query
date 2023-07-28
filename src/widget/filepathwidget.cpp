#include "filepathwidget.h"
#include "ui_filepathwidget.h"
#include "settings.h"
#include <QDir>
#include <QFileDialog>

FilePathWidget::FilePathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilePathWidget)
{
    ui->setupUi(this);
    ui->directory->setText(QDir::toNativeSeparators(Settings::instance()->homePath()));
}

FilePathWidget::~FilePathWidget()
{
    delete ui;
}

bool FilePathWidget::clipboard() const
{
    return ui->outputToClipboard->isChecked();
}

bool FilePathWidget::outputToFile() const {
    return ui->outputToFile->isChecked();
}

QString FilePathWidget::path() const
{
    QString directory = ui->directory->text();
    QString name = ui->file->text();
    return QDir(directory).filePath(name);
}

void FilePathWidget::setPath(const QString& path) {
    QFileInfo info(path);
    ui->directory->setText(QDir::toNativeSeparators(info.absoluteDir().path()));
    //ui->file->setText(info.fileName());
    setName(info.fileName());
}

void FilePathWidget::setName(const QString &name)
{
    ui->file->setText(name);

}

void FilePathWidget::setExt(const QString& ext) {
    mExt = ext;
    QString name = ui->file->text();
    if (name.isEmpty()) {
        return;
    }
    QFileInfo info(name);
    if (mExt.isEmpty()) {
        name = info.baseName();
    } else {
        name = info.baseName() + "." + mExt;
    }
    ui->file->setText(name);
}

void FilePathWidget::setClipboard(bool value)
{
    if (value) {
        ui->outputToClipboard->setChecked(true);
    } else {
        ui->outputToFile->setChecked(true);
    }
}

void FilePathWidget::init(const QString &path, const QString &name, const QString &ext)
{
    if (!path.isEmpty()) {
        setPath(path);
    }
    if (!name.isEmpty()) {
        ui->file->setText(name);
    }
    setExt(ext);
}

void FilePathWidget::on_selectDirectory_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, QString(), ui->directory->text());
    if (path.isEmpty()) {
        return;
    }
    ui->directory->setText(path);
}

void FilePathWidget::on_selectFile_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, QString(), this->path());
    if (path.isEmpty()) {
        return;
    }
    setPath(path);
}
