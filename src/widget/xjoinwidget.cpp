#include "xjoinwidget.h"
#include "ui_xjoinwidget.h"
#include <QDebug>
#include "xjoinmodel.h"
#include <QSqlQueryModel>
#include "history.h"

XJoinWidget::XJoinWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XJoinWidget)
{
    ui->setupUi(this);
}

XJoinWidget::~XJoinWidget()
{
    delete ui;
}

void XJoinWidget::init(const QStringList &connectionNames)
{
    ui->first->init(connectionNames);
    ui->second->init(connectionNames);

    connect(ui->first,&XJoinItemWidget::columnsChanged,this,&XJoinWidget::updateResults);
    connect(ui->second,&XJoinItemWidget::columnsChanged,this,&XJoinWidget::updateResults);
    connect(ui->first,&XJoinItemWidget::queryExecuted,this,&XJoinWidget::updateResults);
    connect(ui->second,&XJoinItemWidget::queryExecuted,this,&XJoinWidget::updateResults);

}

void XJoinWidget::updateResults()
{
    QSqlQueryModel * model1 = ui->first->model();
    QSqlQueryModel * model2 = ui->second->model();

    QStringList columns1 = ui->first->columns();
    QStringList columns2 = ui->second->columns();

    if (!model1 || !model2) {
        return;
    }

    while (model1->canFetchMore()) {
        model1->fetchMore();
    }
    while (model2->canFetchMore()) {
        model2->fetchMore();
    }

    QString query1 = ui->first->query();
    QString query2 = ui->second->query();

    QString connectionName1 = ui->first->connectionName();
    QString connectionName2 = ui->second->connectionName();

    History::instance()->addJoin(connectionName1, query1, columns1,
                                 connectionName2, query2, columns2);

    XJoinModel* model = new XJoinModel(model1, columns1, model2, columns2, this);
    ui->result->setModel(model);
}
