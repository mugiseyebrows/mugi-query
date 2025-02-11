#include "toolmysqldumpdialog.h"
#include "ui_toolmysqldumpdialog.h"

#include <QStandardItemModel>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include "model/checkablemodel.h"
#include "settings.h"
#include <QMessageBox>
#include <QProcess>
#include <QByteArray>
#include "codewidgetdialog.h"

// todo selected tables / all tables

ToolMysqldumpDialog::ToolMysqldumpDialog(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolMysqldumpDialog)/*,
    mProxyModel(new QSortFilterProxyModel(this)),
    mModel(new CheckableModel(this))*/
{
    ui->setupUi(this);

    QStringList tables = db.tables();
    ui->tables->append(tables);

 #if 0
    /*for(int row=0; row<tables.size(); row++) {
        QStandardItem* item = new QStandardItem(tables[row]);
        item->setCheckable(true);
        mModel->appendRow(item);
    }*/
    mModel->append(tables);

    mProxyModel->setSourceModel(mModel);

    ui->tables->setModel(mProxyModel);
#endif

    /*QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
            .filePath(QString("%1_%2").arg(db.databaseName(), QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")));

    ui->output->setText(QDir::toNativeSeparators(path));*/

    ui->output->setText(Settings::instance()->homePath());
    ui->output->setMode(LineSelect::ModeDir);

}

ToolMysqldumpDialog::~ToolMysqldumpDialog()
{
    delete ui;
}

MysqldumpSettings ToolMysqldumpDialog::settings() const
{
    MysqldumpSettings res;
    res.output = ui->output->text();
    res.tables = ui->tables->checked();
    res.format = ui->oneFile->isChecked() ? MysqldumpSettings::OneFile : MysqldumpSettings::MultipleFiles;

    if (ui->pathTable->isChecked()) {
        res.path = MysqldumpSettings::Table;
    } else if (ui->pathDatabaseTable->isChecked()) {
        res.path = MysqldumpSettings::DatabaseTable;
    } else if (ui->pathDatabaseDatetimeTable->isChecked()) {
        res.path = MysqldumpSettings::DatabaseDatetimeTable;
    }
    res.schema = ui->schema->isChecked();
    res.data = ui->data->isChecked();
    res.ssl = ui->ssl->isChecked();
    res.oneFileName = ui->oneFileName->text();

    res.completeInsert = ui->completeInsert->isChecked();
    res.insertIgnore = ui->insertIgnore->isChecked();
    res.extendedInsert = ui->extendedInsert->isChecked();
    res.hexBlob = ui->hexBlob->isChecked();
    res.quoteNames = ui->quoteNames->isChecked();

    return res;
}

#if 0
QString ToolMysqldumpDialog::output() const
{
    return ui->output->text();
}

bool ToolMysqldumpDialog::data() const
{
    return ui->data->isChecked();
}

bool ToolMysqldumpDialog::schema() const
{
    return ui->schema->isChecked();
}

bool ToolMysqldumpDialog::tab() const
{
    return ui->tab->isChecked();
}

bool ToolMysqldumpDialog::ssl() const
{
    return ui->ssl->isChecked();
}

bool ToolMysqldumpDialog::multipleFiles() const
{
    return ui->multipleFiles->isChecked();
}

QStringList ToolMysqldumpDialog::tables() const
{
    /*QAbstractItemModel* model = mModel;
    QStringList tables;
    for(int row=0;row<model->rowCount();row++) {
        auto index = model->index(row, 0);
        if (model->data(index, Qt::CheckStateRole).toInt() == Qt::Checked) {
            tables.append(model->data(index).toString());
        }
    }
    return tables;*/
    return ui->tables->checked();
}
#endif

#if 0
void ToolMysqldumpDialog::on_multipleFiles_clicked(bool checked)
{
    //ui->output->setMode(checked ? LineSelect::ModeDir : LineSelect::ModeFileSave);
}
#endif

#if 0
static void setChecked(QAbstractItemModel* model, Qt::CheckState state) {
    for(int row=0;row<model->rowCount();row++) {
        auto index = model->index(row, 0);
        model->setData(index, state, Qt::CheckStateRole);
    }
}
#endif

#if 0
void ToolMysqldumpDialog::on_checkAll_clicked()
{
    mModel->setChecked(Qt::Checked);
}

void ToolMysqldumpDialog::on_uncheckAll_clicked()
{
    mModel->setChecked(Qt::Unchecked);
}

void ToolMysqldumpDialog::on_filter_textChanged(const QString &text)
{
    mProxyModel->setFilterFixedString(text);
}
#endif

void ToolMysqldumpDialog::accept()
{
    auto output = ui->output->text();
    auto tables = ui->tables->checked();

    if (tables.isEmpty()) {
        QMessageBox::critical(this, QString(), "No files selected");
        return;
    }
#if 0
    if (!QDir(output).exists()) {
        QMessageBox::critical(this, QString(), QString("Directory does not exist %1").arg(output));
        return;
    }
#endif
    QDialog::accept();
}

void ToolMysqldumpDialog::on_multipleFiles_clicked()
{
    ui->oneFileName->setEnabled(false);
}

void ToolMysqldumpDialog::on_oneFile_clicked()
{
    ui->oneFileName->setEnabled(true);
}

static QString checkOutput(const QString& program, const QStringList& args, int timeout, QWidget *widget) {

    QProcess process;
    process.setProgram(program);
    process.setArguments(args);

    process.start(QIODevice::ReadOnly);
    if (!process.waitForStarted()) {
        QMessageBox::critical(widget, QString(), QString("WaitForStarted error %1").arg(process.errorString()));
        return {};
    }
    if (!process.waitForFinished(timeout)) {
        QMessageBox::critical(widget, QString(), QString("WaitForFinished error %1").arg(process.errorString()));
    }
    QByteArray stderrData = process.readAllStandardOutput();
    return QString::fromUtf8(stderrData);
}

void ToolMysqldumpDialog::on_help_clicked()
{
    QString mysqldump = Settings::instance()->mysqldumpPath();
    if (mysqldump.isEmpty() || !QFile::exists(mysqldump)) {
        QMessageBox::critical(0, "Error", "Mysqldump not found");
        return;
    }
    QString output = checkOutput(mysqldump, {"--help"}, 10000, nullptr);
    CodeWidgetDialog dialog(this);
    dialog.setWindowTitle("mysqldump --help");
    dialog.setText(output);
    dialog.exec();
}

