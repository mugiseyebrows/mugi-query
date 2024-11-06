#include "toolmysqldialog.h"
#include "ui_toolmysqldialog.h"
#include <QFileDialog>
#include <QStringListModel>
#include <QDirIterator>
#include <QDebug>
#include <QTimer>
#include "settings.h"

ToolMysqlDialog::ToolMysqlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolMysqlDialog)
{
    ui->setupUi(this);
    mFiles = new QStringListModel();
    ui->files->setModel(mFiles);

    mDir = Settings::instance()->homePath();
}

ToolMysqlDialog::~ToolMysqlDialog()
{
    delete ui;
}

QStringList ToolMysqlDialog::files() const
{
    return mFiles->stringList();
}

bool ToolMysqlDialog::ssl() const
{
    return ui->ssl->isChecked();
}

static QStringList toNativeSeparators(const QStringList& names) {
    QStringList res;
    for(const QString& name: std::as_const(names)) {
        res.append(QDir::toNativeSeparators(name));
    }
    return res;
}

void ToolMysqlDialog::on_addFile_clicked()
{
    QString filter = "Sql files (*.sql);;All files (*.*)";
    auto names = QFileDialog::getOpenFileNames(this, QString(), mDir, filter);
    if (!names.isEmpty()) {
        mFiles->setStringList(mFiles->stringList() + toNativeSeparators(names));
        mDir = QFileInfo(names[0]).dir().absolutePath();
    }
}

void ToolMysqlDialog::on_addDirectory_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this);
    if (path.isEmpty()) {
        return;
    }
    QDirIterator it(path, QDirIterator::Subdirectories);
    mDir = path;
    QStringList names;
    while (it.hasNext()) {
        QString item = it.next();
        QFileInfo info(item);
        if (info.fileName() == "." || info.fileName() == "..") {
            continue;
        }
        names.append(item);
    }
    if (names.isEmpty()) {
        return;
    }
    mFiles->setStringList(mFiles->stringList() + toNativeSeparators(names));
}

