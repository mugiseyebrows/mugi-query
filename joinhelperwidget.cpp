
#include "ui_joinhelperwidget.h"
#include "joinhelperwidget.h"

#include <QDir>
#include "settings.h"
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <QTextStream>
#include <QTextCodec>
#include <QMessageBox>
#include "modelappender.h"
#include "itemdelegatewithcompleter.h"
#include "checkablestringlistmodel.h"
#include "rowvaluegetter.h"
#include <vector>
#include <QDebug>
#include "relation.h"
#include "relations.h"
#include "setsplitersizesratio.h"

#include "relationsmodel.h"
#include <QSortFilterProxyModel>
#include <QTimer>

JoinHelperWidget::JoinHelperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinHelperWidget)
{
    ui->setupUi(this);

    RelationsModel* model = new RelationsModel(this);

    ui->relations->setModel(model);

    mAppender = new ModelAppender(this);
    mAppender->setModel(model);

    QStandardItemModel* selectedTables = new QStandardItemModel(0,1,this);
    ui->selectedTables->setModel(selectedTables);

    QTimer::singleShot(0,this,SLOT(onAdjustSplitters()));
}

void JoinHelperWidget::init(const Tokens& tokens) {

    ItemDelegateWithCompleter* itemDelegate = new ItemDelegateWithCompleter(tokens.fields(true),this);
    ui->relations->setItemDelegate(itemDelegate);

    QStringListModel* allTables = new QStringListModel(tokens.tables(),this);

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(allTables);
    ui->allTables->setModel(proxyModel);

    ui->query->setTokens(tokens);
}

void JoinHelperWidget::closeEvent(QCloseEvent *event)
{
    saveRelationsModel();
    QWidget::closeEvent(event);
}

void JoinHelperWidget::init(const QString& connectionName) {

    if (mConnectionName == connectionName) {
        return;
    }

    saveRelationsModel();
    mConnectionName = connectionName;
    loadRelationsModel();

    QAbstractItemModel* selectedTables = ui->selectedTables->model();
    if (selectedTables->rowCount() > 0) {
        selectedTables->removeRows(0,selectedTables->rowCount());
    }

}

void JoinHelperWidget::onAdjustSplitters() {
    setSpliterSizesRatio(ui->horizontalSplitter,1,1);
    setSpliterSizesRatio(ui->verticalSplitter,3,1);
}

JoinHelperWidget::~JoinHelperWidget()
{
    delete ui;
}

QString JoinHelperWidget::filePath() const
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList props;
    props << "relations" << db.driverName() << db.hostName()
          << db.userName() << db.databaseName();

    QString name = props.join(" ")
            .replace(QRegExp("[^a-z0-9._ -]",Qt::CaseInsensitive)," ")
            .replace(QRegExp("[ ]+")," ") + ".txt";

    return QDir(Settings::instance()->dir()).filePath(name);
}



void JoinHelperWidget::findPath()
{
    Relations relations(ui->relations->model());

    QAbstractItemModel* model = ui->selectedTables->model();

    if (model->rowCount() < 2) {
        ui->query->setText("select two or more tables");
        return;
    }

    QStringList tables;
    for(int row=0;row<model->rowCount();row++) {
        tables << model->data(model->index(row,0)).toString();
    }
    Relations::PathList path = relations.findPath(tables);
    if (path.isEmpty()) {
        ui->query->setText("no path found");
    } else {
        ui->query->setText(relations.expression(path,true,true));
    }

}

void JoinHelperWidget::on_add_clicked()
{
    QModelIndexList indexes = ui->allTables->selectionModel()->selectedIndexes();
    QAbstractItemModel* model = ui->selectedTables->model();
    foreach(const QModelIndex& index, indexes) {
        int row = model->rowCount();
        model->insertRow(row);
        model->setData(model->index(row,0),index.data());
    }
    findPath();
}

void JoinHelperWidget::on_remove_clicked()
{
    QModelIndexList indexes = ui->selectedTables->selectionModel()->selectedIndexes();
    qSort(indexes);
    QAbstractItemModel* model = ui->selectedTables->model();
    for(QModelIndexList::reverse_iterator it = indexes.rbegin(); it != indexes.rend(); it++) {
        model->removeRow(it->row());
    }

    findPath();
}

void JoinHelperWidget::on_filter_textChanged(const QString &text)
{
    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->allTables->model());
    proxyModel->setFilterRegExp(QRegExp(text,Qt::CaseInsensitive));
}

void JoinHelperWidget::on_allTables_doubleClicked(const QModelIndex &index)
{
    QAbstractItemModel* model = ui->selectedTables->model();
    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row,0),index.data());
    findPath();
}

void JoinHelperWidget::on_selectedTables_doubleClicked(const QModelIndex &index)
{
    QAbstractItemModel* model = ui->selectedTables->model();
    model->removeRow(index.row());
    findPath();
}

void JoinHelperWidget::saveRelationsModel() {
    RelationsModel* model = qobject_cast<RelationsModel*>(ui->relations->model());
    model->save(filePath());
}

void JoinHelperWidget::loadRelationsModel()
{
    RelationsModel* model = qobject_cast<RelationsModel*>(ui->relations->model());
    mAppender->setActive(false);
    model->load(filePath());
    model->insertRow(model->rowCount());
    mAppender->setActive(true);
}

void JoinHelperWidget::on_copy_clicked()
{
    saveRelationsModel();
    emit appendQuery(ui->query->toPlainText());
}
