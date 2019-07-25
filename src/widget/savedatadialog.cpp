#include "savedatadialog.h"
#include "ui_savedatadialog.h"

#include "model/dataimportcolumnmodel.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include "error.h"

#include "widget/selectcolumnslistwidget.h"

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

    ui->columns->setFields(fields);
    ui->columns->data()->setAllChecked();
    ui->columns->setLabelsMode(SelectColumnsWidget::LabelsModeLong);

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

QList<bool> SaveDataDialog::keysChecked() const
{
    return keysModel()->checkedAsBoolList();
}

QList<bool> SaveDataDialog::dataChecked() const
{
    return dataModel()->checkedAsBoolList();
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
        Error::show(this,"You need to check at least one data field");
        return;
    }
    if (format() == DataFormat::SqlUpdate) {
        if (!keysModel()->hasAnyChecked()) {
            Error::show(this,"You need to check at least one field for where clause");
            return;
        }
    }
    QDialog::accept();
}

DataImportColumnModel* SaveDataDialog::dataModel() const {
    return ui->columns->data()->model();
}

DataImportColumnModel* SaveDataDialog::keysModel() const {
    return ui->columns->keys()->model();
}

void SaveDataDialog::on_table_textChanged(const QString& table)
{
    ui->columns->setTable(table);
}

void SaveDataDialog::on_format_currentIndexChanged(int)
{
    ui->columns->setFormat(format());
}
