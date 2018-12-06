#include "queryhistorywidget.h"
#include "ui_queryhistorywidget.h"

#include <QSqlQueryModel>

QueryHistoryWidget::QueryHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryHistoryWidget)
{
    ui->setupUi(this);

    QSqlQueryModel* model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    refresh();
    ui->tableView->setColumnWidth(0,160);
}

QueryHistoryWidget::~QueryHistoryWidget()
{
    delete ui;
}

void QueryHistoryWidget::on_tableView_doubleClicked(QModelIndex index) {
    QAbstractItemModel* m = ui->tableView->model();
    QString query = m->data(m->index(index.row(),col_query)).toString();
    emit copyQuery(query);
}

void QueryHistoryWidget::refresh()
{
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    QSqlQuery q("select * from query order by date desc",QSqlDatabase::database("_history"));
    model->setQuery(q);
}

void QueryHistoryWidget::on_refresh_clicked()
{
    refresh();
}

void QueryHistoryWidget::on_copy_clicked()
{
    //emit copyQuery();
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    on_tableView_doubleClicked(index);
}
