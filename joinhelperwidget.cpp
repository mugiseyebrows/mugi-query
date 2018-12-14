
#include "ui_joinhelperwidget.h"
#include "joinhelperwidget.h"

#include <QDir>
#include "settings.h"
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <QStringListModel>
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
#include "sl.h"

JoinHelperWidget::JoinHelperWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinHelperWidget)
{
    ui->setupUi(this);

    RelationsModel* model = new RelationsModel(this);

    ui->relations->setModel(model);

    mRelationsAppender = new ModelAppender(this);
    mRelationsAppender->setModel(model);

    /*QStandardItemModel* selectedTables = new QStandardItemModel(0,1,this);
    ui->selectedTables->setModel(selectedTables);*/

    QStringListModel* tables = new QStringListModel();
    tables->setHeaderData(0,Qt::Horizontal,"Tables");
    ui->tables->setModel(tables);

    mTablesAppender = new ModelAppender(this);
    mTablesAppender->setModel(tables);

    QTimer::singleShot(0,this,SLOT(onAdjustSplitters()));
}

void JoinHelperWidget::update(const Tokens& tokens) {

    ItemDelegateWithCompleter* relationsDelegate = new ItemDelegateWithCompleter(tokens.fields(true),this);
    ui->relations->setItemDelegate(relationsDelegate);

    ItemDelegateWithCompleter* tablesDelegate = new ItemDelegateWithCompleter(tokens.tables(),this);
    ui->tables->setItemDelegate(tablesDelegate);
    ui->query->setTokens(tokens);
}

void JoinHelperWidget::closeEvent(QCloseEvent *event)
{
    saveRelationsModel();
    QWidget::closeEvent(event);
}

void JoinHelperWidget::init(const QString& connectionName) {

    saveRelationsModel();
    mConnectionName = connectionName;
    loadRelationsModel();

    //QAbstractItemModel* selectedTables = ui->selectedTables->model();

    QStringListModel* tables = qobject_cast<QStringListModel*>(ui->tables->model());

    mTablesAppender->setActive(false);

    tables->setStringList(sl(QString(),QString()));

    mTablesAppender->setActive(true);

    connect(mTablesAppender,SIGNAL(rowInserted(int)),this,SLOT(onTablesRowInserted(int)));

    connect(tables,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onTablesDataChanged(QModelIndex,QModelIndex)));
}

void JoinHelperWidget::onTablesDataChanged(QModelIndex,QModelIndex) {
    findPath();
}

void JoinHelperWidget::onTablesRowInserted(int) {
    QTimer::singleShot(0,this,SLOT(onSelectLastTablesRow()));
}

void JoinHelperWidget::onSelectLastTablesRow() {
    QAbstractItemModel* model = ui->tables->model();
    int row = model->rowCount()-1;
    QModelIndex index = model->index(row,0);
    ui->tables->setCurrentIndex(index);
}

void JoinHelperWidget::onAdjustSplitters() {
    setSpliterSizesRatio(ui->horizontalSplitter,1,1);
    setSpliterSizesRatio(ui->verticalSplitter,3,1);
}

JoinHelperWidget::~JoinHelperWidget()
{
    delete ui;
}

QString JoinHelperWidget::relationsPath() const
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


namespace {

QStringList filterEmpty(const QStringList& items) {
    QStringList res;
    foreach(const QString& item, items) {
        if (!item.isEmpty()) {
            res << item;
        }
    }
    return res;
}

}

void JoinHelperWidget::findPath()
{
    Relations relations(ui->relations->model());

    QStringListModel* tablesModel = qobject_cast<QStringListModel*>(ui->tables->model());

    QStringList tables = filterEmpty(tablesModel->stringList());

    if (tables.size() < 2) {
        ui->query->setText("select two or more tables");
        return;
    }

    bool leftJoin = ui->joinType->currentIndex() == 0;
    bool mssql = QSqlDatabase::database(mConnectionName).driverName() == "QODBC";

    Relations::PathList path = relations.findPath(tables);
    if (path.isEmpty()) {
        ui->query->setText("no path found");
    } else {
        ui->query->setText(relations.expression(path,leftJoin,mssql));
    }

}




void JoinHelperWidget::saveRelationsModel() {
    RelationsModel* model = qobject_cast<RelationsModel*>(ui->relations->model());
    model->save(relationsPath());
}

void JoinHelperWidget::loadRelationsModel()
{
    RelationsModel* model = qobject_cast<RelationsModel*>(ui->relations->model());
    mRelationsAppender->setActive(false);
    model->load(relationsPath());
    model->insertRow(model->rowCount());
    mRelationsAppender->setActive(true);
}

void JoinHelperWidget::on_copy_clicked()
{
    saveRelationsModel();
    emit appendQuery(ui->query->toPlainText());
}

void JoinHelperWidget::on_joinType_currentIndexChanged(int)
{
    findPath();
}
