#include "queryhistorywidget.h"
#include "ui_queryhistorywidget.h"

#include <QSqlQueryModel>
#include <QStringListModel>
#include <QDebug>
#include "copyeventfilter.h"
#include "model/queryhistorymodel.h"
#include <QClipboard>

namespace {

QList<int> partialySelectedRows(QItemSelectionModel* selectionModel) {
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QList<int> rows;
    foreach (const QModelIndex& index, indexes) {
        rows << index.row();
    }
    rows = rows.toSet().toList();
    qSort(rows.begin(),rows.end());
    return rows;
}

}


QueryHistoryWidget::QueryHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryHistoryWidget)
{
    ui->setupUi(this);

    QueryHistoryModel* model = new QueryHistoryModel(this);
    ui->tableView->setModel(model);
    refresh("all");
    ui->tableView->setColumnWidth(0,160);

    CopyEventFilter* filter = new CopyEventFilter(this);
    filter->setView(ui->tableView);

    connect(filter,&CopyEventFilter::copy,[=](){
        QStringList queries = selectedQueries();
        if (queries.isEmpty()) {
            return;
        }
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(queries.join(";\n"));
    });

}

QueryHistoryWidget::~QueryHistoryWidget()
{
    delete ui;
}

QStringList QueryHistoryWidget::selectedQueries() const {
    QList<int> rows = partialySelectedRows(ui->tableView->selectionModel());
    QStringList queries;
    QAbstractItemModel* model = ui->tableView->model();
    foreach (int row, rows) {
        queries << model->data(model->index(row,QueryHistoryModel::col_query)).toString();
    }
    return queries;
}

void QueryHistoryWidget::on_tableView_doubleClicked(QModelIndex index) {
    QAbstractItemModel* m = ui->tableView->model();
    QString connectionName = m->data(m->index(index.row(),QueryHistoryModel::col_connectionName)).toString();
    QString query = m->data(m->index(index.row(),QueryHistoryModel::col_query)).toString();
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
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QStringList queries = selectedQueries();
    if (queries.isEmpty()) {
        return;
    }
    QAbstractItemModel* model = ui->tableView->model();
    QString connectionName = model->data(model->index(index.row(),QueryHistoryModel::col_connectionName)).toString();
    emit appendQuery(connectionName,queries.join(";\n"));
}

void QueryHistoryWidget::on_connectionName_currentIndexChanged(const QString &)
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
