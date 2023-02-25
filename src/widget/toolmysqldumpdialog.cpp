#include "toolmysqldumpdialog.h"
#include "ui_toolmysqldumpdialog.h"

#include <QStandardItemModel>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QSortFilterProxyModel>

ToolMysqldumpDialog::ToolMysqldumpDialog(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolMysqldumpDialog),
    mProxyModel(new QSortFilterProxyModel(this)),
    mModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    QStringList tables = db.tables();
    for(int row=0; row<tables.size(); row++) {
        QStandardItem* item = new QStandardItem(tables[row]);
        item->setCheckable(true);
        mModel->appendRow(item);
    }

    mProxyModel->setSourceModel(mModel);

    ui->tables->setModel(mProxyModel);

    QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
            .filePath(QString("%1_%2").arg(db.databaseName(), QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")));

    ui->output->setText(QDir::toNativeSeparators(path));

    //on_multipleFiles_clicked(ui->multipleFiles->isChecked());

    ui->output->setMode(LineSelect::ModeDir);

}

ToolMysqldumpDialog::~ToolMysqldumpDialog()
{
    delete ui;
}

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

bool ToolMysqldumpDialog::multipleFiles() const
{
    return ui->multipleFiles->isChecked();
}

QStringList ToolMysqldumpDialog::tables() const
{
    QAbstractItemModel* model = mModel;
    QStringList tables;
    for(int row=0;row<model->rowCount();row++) {
        auto index = model->index(row, 0);
        if (model->data(index, Qt::CheckStateRole).toInt() == Qt::Checked) {
            tables.append(model->data(index).toString());
        }
    }
    return tables;
}

void ToolMysqldumpDialog::on_multipleFiles_clicked(bool checked)
{
    //ui->output->setMode(checked ? LineSelect::ModeDir : LineSelect::ModeFileSave);
}

static void setChecked(QAbstractItemModel* model, Qt::CheckState state) {
    for(int row=0;row<model->rowCount();row++) {
        auto index = model->index(row, 0);
        model->setData(index, state, Qt::CheckStateRole);
    }
}

void ToolMysqldumpDialog::on_checkAll_clicked()
{
    setChecked(mModel, Qt::Checked);
}

void ToolMysqldumpDialog::on_uncheckAll_clicked()
{
    setChecked(mModel, Qt::Unchecked);
}

void ToolMysqldumpDialog::on_filter_textChanged(const QString &text)
{
    mProxyModel->setFilterFixedString(text);
}

