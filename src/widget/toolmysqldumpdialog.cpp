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
    res.path = ui->formatDatabaseName->isChecked() ? MysqldumpSettings::DatabaseName : MysqldumpSettings::DatabaseDatetimeName;
    res.schema = ui->schema->isChecked();
    res.data = ui->data->isChecked();
    res.ssl = ui->ssl->isChecked();
    res.tab = ui->tab->isChecked();
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
