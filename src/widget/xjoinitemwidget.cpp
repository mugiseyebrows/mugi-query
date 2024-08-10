#include "xjoinitemwidget.h"
#include "ui_xjoinitemwidget.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "itemdelegatewithcompleter.h"
#include <QStandardItemModel>
#include "modelcolumn.h"
#include "fieldnames.h"
#include <QSqlRecord>

XJoinItemWidget::XJoinItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XJoinItemWidget)
{
    ui->setupUi(this);
}

XJoinItemWidget::~XJoinItemWidget()
{
    delete ui;
}


void XJoinItemWidget::on_execute_clicked()
{
    QSqlDatabase db = QSqlDatabase::database(ui->connection->currentText());
    QSqlQuery q(db);
    QString query = ui->query->toPlainText();
    if (!q.exec(query)) {

    } else {
        QSqlQueryModel* model = new QSqlQueryModel(this);
        model->setQuery(std::move(q));
        ui->result->setModel(model);

        QStringList names = fieldNames(model->record());
        auto* delegate = new ItemDelegateWithCompleter(names, ui->columns);
        ui->columns->setItemDelegate(delegate);

        emit queryExecuted();
    }
}

void XJoinItemWidget::init(const QStringList &connectionNames)
{
    ui->connection->addItems(connectionNames);

    QStandardItemModel* model = new QStandardItemModel(1,1);
    ui->columns->setModel(model);

    // todo xjoin on multiple columns

    connect(model, &QAbstractItemModel::dataChanged,
            this, &XJoinItemWidget::columnsChanged);

}

QString XJoinItemWidget::connectionName() const
{
    return ui->connection->currentText();
}

QString XJoinItemWidget::query() const
{
    return ui->query->toPlainText();
}

QSqlQueryModel *XJoinItemWidget::model()
{
    return qobject_cast<QSqlQueryModel*>(ui->result->model());
}

QStringList XJoinItemWidget::columns() const
{
    return modelColumn(ui->columns->model(), 0);
}
