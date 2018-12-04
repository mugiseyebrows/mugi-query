#include "sessiontab.h"
#include "ui_sessiontab.h"

#include <QSqlQueryModel>
#include <QTableView>
#include <QStandardItemModel>
#include <QDebug>
#include "rowvaluesetter.h"
#include "savedatadialog.h"

SessionTab::SessionTab(const QString &connectionName, const QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionTab), mConnectionName(connectionName), mName(name)
{
    ui->setupUi(this);
    cleanTabs();
}

SessionTab::~SessionTab()
{
    delete ui;
}

QString SessionTab::connectionName() const
{
    return mConnectionName;
}

QString SessionTab::name() const
{
    return mName;
}

void SessionTab::setResult(const QStringList& queries, const QStringList errors, const QList<QSqlQueryModel *> models, const QList<int> &perf, const QList<int> &rowsAffected)
{
    cleanTabs();

    int i = 0;
    foreach(QSqlQueryModel * model, models) {
        if (model) {
            QTableView* view = new QTableView();
            view->setModel(model);
            QString title = QString("res %1").arg(++i);
            ui->resultTabs->insertTab(ui->resultTabs->count(),view,title);
        }
    }

    QStandardItemModel* model = new QStandardItemModel(queries.size(),4);

    model->setHeaderData(0,Qt::Horizontal,"query");
    model->setHeaderData(1,Qt::Horizontal,"ms");
    model->setHeaderData(2,Qt::Horizontal,"rows");
    model->setHeaderData(3,Qt::Horizontal,"error");

    for(int i=0;i<queries.size();i++) {
        RowValueSetter s(model,i);
        s(0,queries[i].trimmed());
        s(1,perf[i]);
        s(2,rowsAffected[i]);
        s(3,errors[i]);
    }

    bool hasError = false;
    foreach(const QString error, errors) {
        if (!error.isEmpty()) {
            hasError = true;
        }
    }
    int width = this->width();


    QTableView* view = new QTableView();
    view->verticalHeader()->setDefaultSectionSize(40);
    view->horizontalHeader()->setStretchLastSection(true);
    view->setModel(model);

    int columnWidth = (this->width() - 60 - view->horizontalHeader()->defaultSectionSize() * (hasError ? 2 : 3)) / (hasError ? 2 : 1);

    view->setColumnWidth(0,columnWidth);

    QString title = "stat";
    ui->resultTabs->insertTab(ui->resultTabs->count(),view,title);
}

void SessionTab::setCompleter(QCompleter *completer)
{
    qDebug() << "SessionTab::setCompleter";
    ui->query->setCompleter(completer);
}

void SessionTab::cleanTabs()
{
    while(ui->resultTabs->count() > 0) {
        ui->resultTabs->widget(0)->deleteLater();
        ui->resultTabs->removeTab(0);
    }
}

void SessionTab::setQuery(const QString &query)
{
    ui->query->setPlainText(query);
}

QSqlQueryModel *SessionTab::currentModel()
{
    QTableView* view = qobject_cast<QTableView*>(ui->resultTabs->currentWidget());
    if (!view) {
        return 0;
    }
    return qobject_cast<QSqlQueryModel*>(view->model());
}

void SessionTab::on_execute_clicked()
{
    emit query(ui->query->toPlainText());
}

void SessionTab::on_history_clicked()
{
    emit showQueryHistory();
}

void SessionTab::on_save_clicked()
{
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }
    QStringList fields;
    for(int c=0;c<model->columnCount();c++) {
        fields << model->headerData(c,Qt::Horizontal).toString();
    }

    SaveDataDialog dialog(fields,this);
    if (dialog.exec() == QDialog::Accepted) {

    }
}
