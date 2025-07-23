#include "toolfilesdialog.h"
#include "ui_toolfilesdialog.h"

#include <QStringListModel>
#include "settings.h"
#include <QFileDialog>
#include <QDirIterator>

static QStringList toNativeSeparators(const QStringList& names) {
    QStringList res;
    for(const QString& name: std::as_const(names)) {
        res.append(QDir::toNativeSeparators(name));
    }
    return res;
}

ToolFilesDialog::ToolFilesDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolFilesDialog), mMode(mode)
{
    ui->setupUi(this);

    mFiles = new QStringListModel();
    ui->files->setModel(mFiles);

    mDir = Settings::instance()->homePath();

    switch(mMode) {
    case Mysql:
        ui->pushCsvOptions->hide();
        ui->pushCsvTable->hide();
        break;
    case PushCsv:
        ui->mysqlConnectionOptions->hide();
        ui->mysqlOptions->hide();
        break;
    }

}

ToolFilesDialog::~ToolFilesDialog()
{
    delete ui;
}

QStringList ToolFilesDialog::files() const
{
    return mFiles->stringList();
}

bool ToolFilesDialog::ssl() const
{
    return ui->ssl->isChecked();
}

bool ToolFilesDialog::force() const
{
    return ui->force->isChecked();
}

QString ToolFilesDialog::table() const
{
    return ui->table->text();
}

IfExists ToolFilesDialog::ifExists() const
{
    if (ui->ifExistsFail->isChecked()) {
        return IfExists::Fail;
    }
    if (ui->ifExistsReplace->isChecked()) {
        return IfExists::Replace;
    }
    if (ui->ifExistsAppend->isChecked()) {
        return IfExists::Append;
    }
    return IfExists::Fail;
}

void ToolFilesDialog::append(const QStringList &names)
{
    mFiles->setStringList(mFiles->stringList() + toNativeSeparators(names));
    if (!ui->table->text().isEmpty()) {
        return;
    }
    QString name = mFiles->index(0, 0).data().toString();
    if (name.isEmpty()) {
        return;
    }
    QFileInfo info(name);
    ui->table->setText(info.baseName());
}


void ToolFilesDialog::on_addFile_clicked()
{
    QString filter;
    switch(mMode) {
    case Mysql: filter = "Sql files (*.sql);;All files (*.*)"; break;
    case PushCsv: filter = "Csv files (*.csv);;All files (*.*)"; break;
    }
    auto names = QFileDialog::getOpenFileNames(this, QString(), mDir, filter);
    if (!names.isEmpty()) {
        append(names);
        mDir = QFileInfo(names[0]).dir().absolutePath();
    }
}

void ToolFilesDialog::on_addDirectory_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this);
    if (path.isEmpty()) {
        return;
    }
    QDirIterator it(path, QDirIterator::Subdirectories);
    mDir = path;
    QStringList names;
    QString suffix;
    switch(mMode) {
    case Mysql: suffix = ".sql"; break;
    case PushCsv: suffix = ".csv"; break;
    }
    while (it.hasNext()) {
        QString item = it.next();
        QFileInfo info(item);
        if (info.fileName() == "." || info.fileName() == "..") {
            continue;
        }
        if (info.suffix().toLower() == suffix) {
            names.append(item);
        }
    }
    if (names.isEmpty()) {
        return;
    }
    append(names);
}



