#include "savedatadialog.h"
#include "ui_savedatadialog.h"

#include "checkablestringlistmodel.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>

SaveDataDialog::SaveDataDialog(QSqlQueryModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDataDialog), mModel(model)
{
    ui->setupUi(this);

    QStringList fields;
    for(int c=0;c<model->columnCount();c++) {
        fields << model->headerData(c,Qt::Horizontal).toString();
    }

    QString query = model->query().executedQuery();
    QRegExp rx("from\\s+([^\\s]+)");
    if (query.indexOf(rx) > -1) {
        ui->table->setText(rx.cap(1));
    }

    CheckableStringListModel* m1 = new CheckableStringListModel(fields,this);
    CheckableStringListModel* m2 = new CheckableStringListModel(fields,this);

    m1->setAllChecked();
    ui->data->setModel(m1);
    ui->keys->setModel(m2);

    OutputType::initComboBox(ui->output);
    DataFormat::initComboBox(ui->format);
}

SaveDataDialog::~SaveDataDialog()
{
    delete ui;
}

DataFormat::Format SaveDataDialog::format() const
{
    return DataFormat::value(ui->format);
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

OutputType::Type SaveDataDialog::output() const
{
    return OutputType::value(ui->output);
}

void SaveDataDialog::accept()
{
    if (!dataModel()->hasAnyChecked()) {
        QMessageBox::critical(this,"Error","You need to check at least one data field");
        return;
    }
    if (format() == DataFormat::SqlUpdate) {
        if (!keysModel()->hasAnyChecked()) {
            QMessageBox::critical(this,"Error","You need to check at least one field for where clause");
            return;
        }
    }

    QDialog::accept();
}

CheckableStringListModel* SaveDataDialog::dataModel() const {
    return qobject_cast<CheckableStringListModel*>(ui->data->model());
}

CheckableStringListModel* SaveDataDialog::keysModel() const {
    return qobject_cast<CheckableStringListModel*>(ui->keys->model());
}

void SaveDataDialog::on_allData_clicked()
{
    dataModel()->setAllChecked();
}

void SaveDataDialog::on_noneData_clicked()
{
    dataModel()->setAllUnchecked();
}

void SaveDataDialog::on_allKeys_clicked()
{
    keysModel()->setAllChecked();
}

void SaveDataDialog::on_noneKeys_clicked()
{
    keysModel()->setAllUnchecked();
}

void SaveDataDialog::updateLabels() {
    DataFormat::Format f = format();
    QString t = table();

    QString dataLabel;
    QString keysLabel;
    if (f == DataFormat::SqlInsert) {
        dataLabel = QString("insert into %1 (...)").arg(t);
        keysLabel = QString();
    } else if (f == DataFormat::SqlUpdate) {
        dataLabel = QString("update %1 set ...").arg(t);
        keysLabel = "where ...";
    } else if (f == DataFormat::Csv || t == DataFormat::Tsv) {
        dataLabel = t;
        keysLabel = QString();
    }
    ui->dataLabel->setText(dataLabel);
    ui->keysLabel->setText(keysLabel);
}

void SaveDataDialog::on_table_textChanged(const QString&)
{
    updateLabels();
}

void SaveDataDialog::on_format_currentIndexChanged(int)
{
    updateLabels();
}
