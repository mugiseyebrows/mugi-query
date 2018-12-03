#include "queryhistorywidget.h"
#include "ui_queryhistorywidget.h"

#include <QSqlTableModel>

QueryHistoryWidget::QueryHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryHistoryWidget)
{
    ui->setupUi(this);

    QSqlTableModel* model = new QSqlTableModel(this,QSqlDatabase::database("_history"));
    model->setTable("query");
    model->select();
    ui->tableView->setModel(model);

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
    QSqlTableModel* model = qobject_cast<QSqlTableModel*>(ui->tableView->model());
    model->select();
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
