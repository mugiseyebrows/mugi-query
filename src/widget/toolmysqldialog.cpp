#include "toolmysqldialog.h"
#include "ui_toolmysqldialog.h"
#include <QFileDialog>
#include <QStringListModel>
#include <QDirIterator>
#include <QDebug>
#include <QTimer>

ToolMysqlDialog::ToolMysqlDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolMysqlDialog)
{
    ui->setupUi(this);
    mModel = new QStringListModel();
    ui->inputs->setModel(mModel);

    connect(ui->oneFile, &QPushButton::clicked, [=](bool checked){
        setMode(checked ? OneFile : MultipleFiles);
    });

    connect(ui->multipleFiles, &QPushButton::clicked, [=](bool checked){
        setMode(checked ? MultipleFiles : OneFile);
    });
    setMode(mode);
}

ToolMysqlDialog::~ToolMysqlDialog()
{
    delete ui;
}

QStringList ToolMysqlDialog::inputs() const
{
    if (mMode == OneFile) {
        QString path = ui->input->text();
        if (path.isEmpty()) {
            return {};
        }
        return {path};
    }
    return mModel->stringList();
}

void ToolMysqlDialog::setMode(Mode mode)
{
    mMode = mode;
    ui->oneFile->setChecked(mode == OneFile);
    ui->multipleFiles->setChecked(mode == MultipleFiles);
    ui->groupOneFile->setVisible(mode == OneFile);
    ui->groupMultipleFiles->setVisible(mode == MultipleFiles);
    QTimer::singleShot(0, [=](){
        QRect rect = geometry();
        rect.setSize(QSize(600, mode == OneFile ? 200 : 400));
        setGeometry(rect);
    });
}

static QStringList toNativeSeparators(const QStringList& names) {
    QStringList res;
    for(const QString& name: qAsConst(names)) {
        res.append(QDir::toNativeSeparators(name));
    }
    return res;
}

void ToolMysqlDialog::on_addFile_clicked()
{
    QString filter = "Sql files (*.sql);;All files (*.*)";
    auto names = QFileDialog::getOpenFileNames(this, QString(), QString(), filter);
    if (!names.isEmpty()) {
        mModel->setStringList(mModel->stringList() + toNativeSeparators(names));
    }
}

void ToolMysqlDialog::on_addDirectory_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this);
    if (path.isEmpty()) {
        return;
    }
    QDirIterator it(path, QDirIterator::Subdirectories);
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
    mModel->setStringList(mModel->stringList() + toNativeSeparators(names));
}

