#include "databasehistorydialog.h"
#include "ui_databasehistorydialog.h"

#include <QSqlTableModel>
#include <QVariant>

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

QVariant DatabaseHistoryDialog::data(int column) const
{
    QAbstractItemModel* m = ui->tableView->model();
    return m->data(m->index(ui->tableView->currentIndex().row(),column));
}

QString DatabaseHistoryDialog::dataToString(int column) const
{
    return data(column).toString();
}

int DatabaseHistoryDialog::dataToInt(int column) const
{
    return data(column).toInt();
}

void DatabaseHistoryDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    QDialog::accept();
}

QString DatabaseHistoryDialog::connectionName() const {
    return dataToString(col_connectionName);
}

QString DatabaseHistoryDialog::driver() const {
    return dataToString(col_driver);
}

QString DatabaseHistoryDialog::host() const
{
    return dataToString(col_host);
}

QString DatabaseHistoryDialog::user() const
{
    return dataToString(col_user);
}

QString DatabaseHistoryDialog::password() const
{
    return dataToString(col_password);
}

QString DatabaseHistoryDialog::database() const
{
    return dataToString(col_database);
}

int DatabaseHistoryDialog::port() const
{
    return dataToInt(col_port);
}

