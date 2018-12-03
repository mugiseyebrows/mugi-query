#include "databasehistorydialog.h"
#include "ui_databasehistorydialog.h"

#include <QSqlTableModel>
#include <QVariant>

#define MODEL (ui->tableView->model())
#define MODEL_DATA_STRING(col) MODEL->data(MODEL->index(ui->tableView->currentIndex().row(),col)).toString()
#define MODEL_DATA_INT(col) MODEL->data(MODEL->index(ui->tableView->currentIndex().row(),col)).toInt()

DatabaseHistoryDialog::DatabaseHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseHistoryDialog)
{
    ui->setupUi(this);

    QSqlTableModel* model = new QSqlTableModel(this,QSqlDatabase::database("_history"));
    model->setTable("database");
    model->select();
    ui->tableView->setModel(model);

    ui->tableView->setColumnWidth(0,160);

}

DatabaseHistoryDialog::~DatabaseHistoryDialog()
{
    delete ui;
}

void DatabaseHistoryDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    QDialog::accept();
}

QString DatabaseHistoryDialog::connectionName() const {
    return MODEL_DATA_STRING(1);
}

QString DatabaseHistoryDialog::driver() const {
    return MODEL_DATA_STRING(2);
}

QString DatabaseHistoryDialog::host() const
{
    return MODEL_DATA_STRING(3);
}

QString DatabaseHistoryDialog::user() const
{
    return MODEL_DATA_STRING(4);
}

QString DatabaseHistoryDialog::password() const
{
    return MODEL_DATA_STRING(5);
}

QString DatabaseHistoryDialog::database() const
{
    return MODEL_DATA_STRING(6);
}

int DatabaseHistoryDialog::port() const
{
    return MODEL_DATA_INT(7);
}

