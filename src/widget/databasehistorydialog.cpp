#include "databasehistorydialog.h"
#include "ui_databasehistorydialog.h"

#include <QSqlQueryModel>
#include <QVariant>
#include <QSqlQuery>

DatabaseHistoryDialog::DatabaseHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseHistoryDialog)
{
    ui->setupUi(this);

    QSqlQueryModel* model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    refresh();
}

DatabaseHistoryDialog::~DatabaseHistoryDialog()
{
    delete ui;
}

void DatabaseHistoryDialog::refresh() {

    QSqlQuery query("select * from database order by date desc", QSqlDatabase::database("_history"));
    qobject_cast<QSqlQueryModel*>(ui->tableView->model())->setQuery(query);
    ui->tableView->setColumnWidth(0,160);
}

void DatabaseHistoryDialog::select(const QString &connectionName)
{
    QAbstractItemModel* m = ui->tableView->model();
    for(int row=0;row<m->rowCount();row++) {
        if (m->data(m->index(row,col_connectionName)).toString() == connectionName) {
            ui->tableView->setCurrentIndex(m->index(row,0));
            on_tableView_doubleClicked(QModelIndex());
            return;
        }
    }
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

