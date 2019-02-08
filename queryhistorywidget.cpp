#include "queryhistorywidget.h"
#include "ui_queryhistorywidget.h"

#include <QSqlQueryModel>
#include <QStringListModel>
#include <QDebug>

QueryHistoryWidget::QueryHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryHistoryWidget)
{
    ui->setupUi(this);

    QSqlQueryModel* model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    refresh("all");
    ui->tableView->setColumnWidth(0,160);
}

QueryHistoryWidget::~QueryHistoryWidget()
{
    delete ui;
}

void QueryHistoryWidget::on_tableView_doubleClicked(QModelIndex index) {
    QAbstractItemModel* m = ui->tableView->model();
    QString connectionName = m->data(m->index(index.row(),col_connectionName)).toString();
    QString query = m->data(m->index(index.row(),col_query)).toString();
    emit appendQuery(connectionName,query);
}

void QueryHistoryWidget::refresh(const QString& connectionName)
{
    /*QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    QSqlQuery q(QSqlDatabase::database("_history"));
    q.exec("select * from query order by date desc");
    model->setQuery(q);*/

    QSqlQuery q(QSqlDatabase::database("_history"));
    QStringList connectionNames;
    connectionNames << "all";
    q.exec("select distinct connectionName from query");
    while (q.next()) {
        connectionNames << q.value(0).toString();
    }
    ui->connectionName->setModel(new QStringListModel(connectionNames,this));
    int index = connectionNames.indexOf(connectionName);
    if (index > -1) {
        ui->connectionName->setCurrentIndex(index);
    }
    updateQuery();
}

void QueryHistoryWidget::on_refresh_clicked()
{
    QString connectionName = ui->connectionName->currentText();
    refresh(connectionName);
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

void QueryHistoryWidget::on_connectionName_currentIndexChanged(const QString &arg1)
{
    updateQuery();
}

void QueryHistoryWidget::on_search_clicked()
{
    updateQuery();
}

void QueryHistoryWidget::on_all_clicked()
{
    ui->filter->setText("");
    updateQuery();
}

void QueryHistoryWidget::updateQuery() {

    QStringList conditions;
    QVariantList binds;
    conditions << "?";
    binds << "1";
    QString connectionName = ui->connectionName->currentText();
    if (connectionName != "all") {
        conditions << "connectionName=?";
        binds << connectionName;
    }
    QString filter = ui->filter->text();
    if (!filter.isEmpty()) {
        conditions << "query like ?";
        binds << ("%" + filter + "%");
    }
    QSqlQuery q(QSqlDatabase::database("_history"));
    q.prepare(QString("select * from query where %1 order by date desc").arg(conditions.join(" and ")));
    foreach(const QVariant& v, binds) {
        q.addBindValue(v);
    }
    q.exec();
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    if (!model) {
        qDebug() << "not model";
        return;
    }
    model->setQuery(q);
}
