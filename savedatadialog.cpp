#include "savedatadialog.h"
#include "ui_savedatadialog.h"

#include "checkablestringlistmodel.h"
#include <QSqlQueryModel>

SaveDataDialog::SaveDataDialog(QSqlQueryModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDataDialog), mModel(model)
{
    ui->setupUi(this);

    QStringList fields;
    for(int c=0;c<model->columnCount();c++) {
        fields << model->headerData(c,Qt::Horizontal).toString();
    }

    CheckableStringListModel* m1 = new CheckableStringListModel(fields,this);
    CheckableStringListModel* m2 = new CheckableStringListModel(fields,this);

    m1->setAllChecked();
    ui->data->setModel(m1);
    ui->keys->setModel(m2);

}

SaveDataDialog::~SaveDataDialog()
{
    delete ui;
}

DataFormat::Format SaveDataDialog::format() const
{
    QString format = ui->format->currentText();
    QString statement = ui->statement->currentText();

    if (format == "csv") {
        return DataFormat::Csv;
    } else if (format == "sql") {
        if (statement == "insert") {
            return DataFormat::SqlInsert;
        } else if (statement == "update") {
            return DataFormat::SqlUpdate;
        }
    }
    return DataFormat::Unknown;
}

namespace  {

QList<bool> getChecked(QAbstractItemModel* model) {
    QList<bool> res;
    for(int r=0;r<model->rowCount();r++) {
        res << (model->data(model->index(r,0),Qt::CheckStateRole).toInt() == Qt::Checked);
    }
    return res;
}

}

QList<bool> SaveDataDialog::keysChecked() const
{
    return getChecked(ui->keys->model());
}

QList<bool> SaveDataDialog::dataChecked() const
{
    return getChecked(ui->data->model());
}

QString SaveDataDialog::table() const
{
    return ui->table->text();
}

void SaveDataDialog::on_allData_clicked()
{
    qobject_cast<CheckableStringListModel*>(ui->data->model())->setAllChecked();
}

void SaveDataDialog::on_noneData_clicked()
{
    qobject_cast<CheckableStringListModel*>(ui->data->model())->setAllUnchecked();
}

void SaveDataDialog::on_allKeys_clicked()
{
    qobject_cast<CheckableStringListModel*>(ui->keys->model())->setAllChecked();
}

void SaveDataDialog::on_noneKeys_clicked()
{
    qobject_cast<CheckableStringListModel*>(ui->keys->model())->setAllUnchecked();
}

