#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>
#include <algorithm>
#include <numeric>
#include <QDesktopServices>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QCompleter>
#include <QTime>
#include <QSqlQueryModel>
#include <QStringListModel>
#include <QCursor>
#include <QCloseEvent>
#include <QSqlDriver>

#include "sessionmodel.h"
#include "sessiontab.h"
#include "databaseconnectdialog.h"
#include "history.h"
#include "queryparser.h"
#include "queryhistorywidget.h"
#include "splitterutil.h"
#include "settings.h"
#include "highlighter.h"
#include "joinhelperwidget.h"
#include "settingsdialog.h"
#include "joinhelperwidgets.h"
#include "automation.h"
#include "model/schemamodel.h"
#include "widget/dataimportwidgets.h"
#include "drivernames.h"
#include "model/relationsmodel.h"
#include "relations.h"
#include "error.h"

#include <QThread>
#include <QSqlRecord>
#include <QSqlField>
#include "datautils.h"
using namespace DataUtils;

namespace {

void showOnTop(QWidget* widget) {
    widget->show();
    widget->activateWindow();
    widget->raise();
}

void recordToNamesTypes(const QSqlRecord& record, QStringList& names, QList<QVariant::Type>& types) {
    for(int c=0;c<record.count();c++) {
        names << record.fieldName(c);
        types << record.field(c).type();
    }
}

}

#if 0
#include "automate_p.cpp"
#else
void automate(QWidget* widget) {

}
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mQueryHistory(nullptr),
    mJoinHelpers(nullptr),
    mDataImport(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qDebug() << "MainWindow::MainWindow" << QThread::currentThreadId();

    setWindowTitle(QString("%1 %2").arg(qApp->applicationName()).arg(qApp->applicationVersion()));

    while (ui->sessionTabs->count() > 0) {
        ui->sessionTabs->removeTab(0);
    }

    mHistory = new History(this);

    SessionModel* sessionModel = new SessionModel(ui->sessionTree);
    ui->sessionTree->setModel(sessionModel);

    ui->schemaTree->setModel(new SchemaModel(ui->schemaTree));

    connect(sessionModel,
            SIGNAL(sessionAdded(QString,QString,QString)),
            this,
            SLOT(onSessionAdded(QString,QString,QString)));
    connect(sessionModel,
            SIGNAL(sessionRemoved(QString,QString)),
            this,
            SLOT(onSessionRemoved(QString,QString)));

    connect(ui->sessionTree->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(onTreeCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->sessionTabs,SIGNAL(currentChanged(int)),this,SLOT(onTabsCurrentChanged(int)));

    QTimer::singleShot(0,this,SLOT(onAdjustSplitter()));

    automate(this);
}

QAction* cloneAction(QAction* src, const QString& text, QObject* parent) {
    QAction* action = new QAction(text,parent);
    QObject::connect(action,SIGNAL(triggered()),src,SIGNAL(triggered()));
    return action;
}

void MainWindow::on_sessionTree_customContextMenuRequested(QPoint pos) {

    Q_UNUSED(pos);

    QModelIndex index = ui->sessionTree->currentIndex();

    QMenu menu(this);

    QAction* databaseConnect = cloneAction(ui->databaseConnect,"&Connect",&menu);
    QAction* databaseDisconnect = cloneAction(ui->databaseDisconnect,"&Disconnect",&menu);
    QAction* databaseReconnect = ui->databaseReconnect;
    QAction* sessionAdd = cloneAction(ui->sessionAdd,"Add &session",&menu);
    QAction* sessionRemove = cloneAction(ui->sessionRemove,"&Remove session",&menu);

    SessionModel* m = this->model();

    if (!index.isValid()) {

        menu.addAction(databaseConnect);

    } else if (m->isDatabase(index)) {

        menu.addAction(sessionAdd);
        menu.addSeparator();
        menu.addAction(databaseConnect);
        menu.addAction(databaseDisconnect);
        menu.addAction(databaseReconnect);

    } else if (m->isSession(index)) {

        menu.addAction(sessionAdd);
        menu.addAction(sessionRemove);
        menu.addSeparator();
        menu.addAction(databaseConnect);

    }

    menu.exec(QCursor::pos());
}

void MainWindow::onAdjustSplitter() {
    SplitterUtil::setRatio(ui->horizontalSplitter,{1,4});
    SplitterUtil::setRatio(ui->verticalSplitter,{1,3});
}

void MainWindow::onTreeCurrentChanged(QModelIndex index,QModelIndex) {

    if (!index.isValid()) {
        return;
    }

    SessionModel* m = model();

    QString name;

    if (!m->isSession(index)) {
        return;
    }
    name = m->data(index).toString();
    selectTab(ui->sessionTabs,name);
}

void MainWindow::selectTab(const QString& name) {
    selectTab(ui->sessionTabs,name);
}

QMenu *MainWindow::selectionMenu() const
{
    return ui->menuSelection;
}

void MainWindow::selectTab(QTabWidget* widget, const QString& name) {
    int index = tabIndex(widget,name);
    if (index < 0) {
        return;
    }
    if (widget->currentIndex() != index) {
        widget->setCurrentIndex(index);
    }
}

SessionTab *MainWindow::tab(int index)
{
    return qobject_cast<SessionTab*>(ui->sessionTabs->widget(index));
}

int MainWindow::lastTabIndex(const QString& connectionName) {
    int index = -1;
    for(int i=0;i<ui->sessionTabs->count();i++) {
        if (tab(i)->connectionName() == connectionName) {
            index = i;
        }
    }
    return index;
}

void MainWindow::selectDatabase(const QString& connectionName) {
    SessionModel* m = model();
    QModelIndex index = m->indexOf(connectionName);
    ui->sessionTree->setCurrentIndex(index);
}

SessionTab *MainWindow::currentTab()
{
    int index = ui->sessionTabs->currentIndex();
    if (index < 0) {
        return 0;
    }
    return tab(index);
}

void MainWindow::updateSchemaModel() {
    SchemaModel* schemaModel = this->schemaModel();
    schemaModel->update(mTokens);
}

void MainWindow::updateTokens(const QString &connectionName)
{
    mTokens[connectionName] = Tokens(QSqlDatabase::database(connectionName));
    updateSchemaModel();
}

int MainWindow::tabIndex(QTabWidget* widget, const QString& name) {
    int count = widget->count();
    for(int i=0;i<count;i++) {
        if (widget->tabText(i) == name) {
            return i;
        }
    }
    return -1;
}


void MainWindow::onTabsCurrentChanged(int tabIndex) {
    //qDebug() << "onTabsCurrentChanged" << index;

    if (tabIndex < 0) {
        setWindowTitle(QString("%1 %2").arg(qApp->applicationName()).arg(qApp->applicationVersion()));
        return;
    }

    QString connectionName = tab(tabIndex)->connectionName();
    QString name = tab(tabIndex)->name();
    QModelIndex index = model()->indexOf(connectionName,name);
    if (ui->sessionTree->currentIndex() != index) {
        ui->sessionTree->setCurrentIndex(index);
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    setWindowTitle(DataUtils::windowTitle(QString(),db,QString()));

    tab(tabIndex)->focusQuery();

    SchemaModel* schemaModel = this->schemaModel();
    QModelIndex rootIndex = schemaModel->find(connectionName);
    if (rootIndex != ui->schemaTree->rootIndex()) {
        ui->schemaTree->setRootIndex(rootIndex);
    }

}

SchemaModel* MainWindow::schemaModel() {
    return qobject_cast<SchemaModel*>(ui->schemaTree->model());
}

void MainWindow::onSessionAdded(QString connectionName, QString name, QString namePrev) {

    int index;
    if (namePrev.isEmpty()) {
        index = -1;
    } else {
        index = tabIndex(ui->sessionTabs,namePrev);
    }

    //qDebug() << "onSessionAdded" << connectionName << name << namePrev << index;

    SessionTab* tab = new SessionTab(connectionName, name, ui->sessionTabs);

    connect(tab,SIGNAL(showQueryHistory()),this,SLOT(onShowQueryHistory()));

    ui->sessionTabs->insertTab(index+1,tab,name);
    connect(tab,SIGNAL(query(QString)),this,SLOT(onQuery(QString)));

    connect(tab,&SessionTab::appendQuery,[=](QString query){
        onAppendQuery(connectionName,query);
    });

    tab->setTokens(mTokens[connectionName]);

    ui->sessionTabs->setCurrentIndex(ui->sessionTabs->indexOf(tab));

    if (!mQuery.isEmpty()) {
        tab->setQuery(mQuery);
        mQuery = QString();
    }

}


void MainWindow::onAppendQuery(const QString& connectionName, QString query)
{

    mQuery = query;
    selectDatabase(connectionName);
    on_sessionAdd_triggered();
    showOnTop(this);
}

void MainWindow::onShowQueryHistory() {
    if (!mQueryHistory) {
        mQueryHistory = new QueryHistoryWidget();
        connect(mQueryHistory,SIGNAL(appendQuery(QString,QString)),
                this,SLOT(onAppendQuery(QString,QString)));
    }
    mQueryHistory->refresh(currentTab()->connectionName());
    showOnTop(mQueryHistory);
}

QStringList filterBlank(const QStringList items) {
    QStringList res;
    foreach(QString item,items) {
        if (!item.trimmed().isEmpty()) {
            res << item;
        }
    }
    return res;
}

void MainWindow::onQuery(QString queries) {

    SessionTab* tab = currentTab();

    QString connectionName = tab->connectionName();

    QStringList queries_ = filterBlank(QueryParser::split(queries));

    QStringList errors;
    QList<QSqlQueryModel*> models;
    QList<int> perf;
    QList<int> rowsAffected;

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QTime time;
    time.start();

    bool schemaChanged = false;

    foreach(QString query, queries_) {

        mHistory->addQuery(connectionName,query.trimmed());

        schemaChanged = schemaChanged | QueryParser::isAlterSchemaQuery(query);

        QSqlQuery q(db);
        QSqlQueryModel* model = 0;
        QString error;
        int rowsAffected_ = -1;
        if (!q.exec(query)) {
            error = q.lastError().text();
        } else {
            if (q.isSelect()) {
                model = new QSqlQueryModel();
                model->setQuery(q);
            }
            rowsAffected_ = q.numRowsAffected();
        }
        perf << time.restart();
        models << model;
        errors << error;
        rowsAffected << rowsAffected_;
    }

    tab->setResult(queries_,errors,models,perf,rowsAffected);

    if (schemaChanged) {
        updateTokens(connectionName);
        pushTokens(connectionName);
    }

}

void MainWindow::pushTokens(const QString &connectionName)
{
    const Tokens& tokens = mTokens[connectionName];
    for(int i=0;i<ui->sessionTabs->count();i++) {
        SessionTab* tab = this->tab(i);
        if (tab->connectionName() == connectionName) {
            tab->setTokens(tokens);
        }
    }
    /*JoinHelperWidget* helper = mJoinHelpers[connectionName];
    if (helper) {
        helper->update(tokens);
    }*/
    if (mJoinHelpers) {
        mJoinHelpers->update(connectionName,tokens,false);
    }
    if (mDataImport) {
        mDataImport->update(connectionName,tokens);
    }
}



void MainWindow::databaseConnect(bool showHistory)
{
    SessionModel* m = model();
    DatabaseConnectDialog dialog(showHistory, this);
    QString connectionName;

    Automation::instance()->beforeDialog(&dialog);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    connectionName = dialog.connectionName();
    m->addDatabase(connectionName);
    mHistory->addDatabase(connectionName,dialog.driver(),dialog.host(),dialog.user(),dialog.password(),dialog.database(),dialog.port());
    updateTokens(connectionName);
    ui->sessionTree->setCurrentIndex(m->index(m->rowCount()-1,0));
    on_sessionAdd_triggered();

    Automation::instance()->afterDialog(&dialog);
}

void MainWindow::on_databaseConnect_triggered()
{
    databaseConnect(false);
}

void MainWindow::onSessionRemoved(QString connectionName, QString name) {
    //qDebug() << "onSessionRemoved" << name;

    for(int i=0;i<ui->sessionTabs->count();i++) {
        SessionTab* tab = this->tab(i);
        if (tab->connectionName() == connectionName && tab->name() == name) {
            ui->sessionTabs->widget(i)->deleteLater();
            ui->sessionTabs->removeTab(i);
            return;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

SessionModel *MainWindow::model() const
{
    return qobject_cast<SessionModel*>(ui->sessionTree->model());
}

void MainWindow::on_databaseHistory_triggered()
{
    databaseConnect(true);
}

void MainWindow::on_queryHistory_triggered()
{
    onShowQueryHistory();
}

void MainWindow::on_sessionAdd_triggered()
{
    SessionModel* m = model();
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!index.isValid()) {
        return;
    }
    m->addSession(index);
    ui->sessionTree->expandAll();
}

void MainWindow::on_sessionRemove_triggered()
{
    SessionModel* m = model();
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!m->isSession(index)) {
        return;
    }
    m->removeSession(index);
}

void MainWindow::on_databaseDisconnect_triggered()
{
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!index.isValid()) {
        return;
    }

    SessionModel* m = model();
    QString connectionName = m->connectionName(index);
    QString question = QString("Disconnect from %1?").arg(connectionName);

    if (QMessageBox::question(this,"Disconnect",question,QMessageBox::Yes, QMessageBox::Cancel) != QMessageBox::Yes) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    db.close();
    QSqlDatabase::removeDatabase(connectionName);

    m->removeDatabase(index);
    if (mJoinHelpers) {
        mJoinHelpers->closeTab(connectionName);
    }

    mTokens.remove(connectionName);
    updateSchemaModel();
}

QString MainWindow::connectionName() const {
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!index.isValid()) {
        return QString();
    }
    SessionModel* m = model();
    return m->connectionName(index);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mQueryHistory) {
        mQueryHistory->close();
    }

    if (mJoinHelpers) {
        mJoinHelpers->save();
        mJoinHelpers->close();
    }

    Settings::instance()->save();
    event->accept();
    QMainWindow::closeEvent(event);
}

void MainWindow::on_databaseReconnect_triggered()
{
    QString connectionName = this->connectionName();
    if (connectionName.isEmpty()) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    db.close();
    if (!db.open()) {
        QMessageBox::critical(this,"Error",db.lastError().text());
    }
}

void MainWindow::on_queryHelp_triggered()
{
    QString connectionName = this->connectionName();
    if (connectionName.isEmpty()) {
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QString driverName = db.driverName();

    QMap<QString,QString> urls;
    urls[DRIVER_MYSQL] = "https://dev.mysql.com/doc/refman/8.0/en/sql-syntax-data-manipulation.html";
    urls[DRIVER_ODBC] = "https://docs.microsoft.com/en-us/previous-versions/office/developer/office-2007/bb208930(v%3doffice.12)";
    urls[DRIVER_SQLITE] = "https://www.sqlite.org/lang.html";
    urls[DRIVER_PSQL] = "https://www.postgresql.org/docs/current/queries.html";
    if (urls.contains(driverName)) {
        QDesktopServices::openUrl(QUrl(urls[driverName]));
    }

}

void MainWindow::on_selectionCopy_triggered()
{
    //qDebug() << "on_selectionCopy_triggered";
    copySelected(false);
}

void MainWindow::on_selectionCopyAsList_triggered()
{
    //qDebug() << "on_selectionCopyAsList_triggered";
    copySelected(true);
}


void MainWindow::copySelected(bool asList)
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->copySelected(asList);
}

void MainWindow::on_dataSave_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->saveData();
}

void MainWindow::on_queryJoin_triggered()
{
    QString connectionName = this->connectionName();
    if (connectionName.isEmpty()) {
        return;
    }

    if (!mJoinHelpers) {
        mJoinHelpers = new JoinHelperWidgets();
        connect(mJoinHelpers,SIGNAL(appendQuery(QString,QString)),this,SLOT(onAppendQuery(QString,QString)));
    }

    mJoinHelpers->update(connectionName,mTokens[connectionName],true);
    showOnTop(mJoinHelpers);
}

void MainWindow::on_dataImport_triggered()
{
    QString connectionName = this->connectionName();
    if (connectionName.isEmpty()) {
        return;
    }
    if (!mDataImport) {
        mDataImport = new DataImportWidgets();
        connect(mDataImport,SIGNAL(appendQuery(QString,QString)),this,SLOT(onAppendQuery(QString,QString)));
    }

    mDataImport->create(connectionName,mTokens[connectionName]);

}

void MainWindow::on_queryQuote_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->quoteQuery();
}

void MainWindow::on_queryUnquote_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->unquoteQuery();
}

void MainWindow::on_settingsFormat_triggered()
{
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {

    }
}

void MainWindow::on_queryExecute_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->on_execute_clicked();
}

#include "widget/selectcolumnsdialog.h"

void MainWindow::on_schemaTree_customContextMenuRequested(const QPoint &)
{

    QMenu menu;

    /*QAction* alter = new QAction("Alter table",&menu);
    menu.addAction(alter);*/

    QAction* select = new QAction("Select",&menu);
    menu.addAction(select);

    QAction* join = new QAction("Join",&menu);
    menu.addAction(join);

    QAction* update = new QAction("Update",&menu);
    menu.addAction(update);

    QAction* refresh = new QAction("Refresh",&menu);
    menu.addAction(refresh);

    QAction* drop = new QAction("Drop",&menu);
    menu.addAction(drop);

    QAction* result = menu.exec(QCursor::pos());

#if 0
    if (result == alter) {

        QModelIndex index = ui->schemaTree->currentIndex();
        SchemaModel* model = qobject_cast<SchemaModel*>(ui->schemaTree->model());
        if (!model) {
            qDebug() << __FILE__ << __LINE__;
            return;
        }
        if (!model->isTable(index)) {
            return;
        }

        QString connectionName = model->data(index.parent()).toString();
        QString table = model->data(index).toString();

        qDebug() << connectionName << table;
    }
#endif

    if (result == select) {

        QString connectionName = this->connectionName();
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        QSqlDriver* driver = db.driver();
        bool mssql = db.driverName() == DRIVER_ODBC;

        QStringList tables = schemaTreeSelectedTables();
        if (tables.isEmpty()) {
            return;
        }
        QStringList queries;
        foreach(const QString& table, tables) {
            QString query = QString(mssql ? "SELECT TOP 100 * FROM %1" : "SELECT * FROM %1 LIMIT 100").arg(driver->escapeIdentifier(table,QSqlDriver::TableName));
            queries << query;
        }
        onAppendQuery(connectionName,queries.join(";\n"));
    }

    if (result == join) {

        QStringList tables = schemaTreeSelectedTables();
        if (tables.isEmpty()) {
            return;
        }

        QString connectionName = this->connectionName();
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        bool mssql = db.driverName() == DRIVER_ODBC;

        if (tables.size() == 1) {
            QString query = QString("SELECT * FROM %1").arg(tables[0]);
            onAppendQuery(connectionName, query);
            return;
        }

        // TODO relations model singleton
        bool leftJoin = true;
        RelationsModel* relationsModel = new RelationsModel(this);
        relationsModel->load(RelationsModel::path(connectionName));
        Relations relations(relationsModel);
        Relations::PathList path = relations.findPath(tables);
        if (path.isEmpty()) {
            QString error = "No path found";
            Error::show(this,error);
            return;
        }
        QString query = relations.expression(path,leftJoin,mssql);
        onAppendQuery(connectionName, query);
    }

    if (result == drop) {
        QString connectionName = this->connectionName();
        QStringList tables = schemaTreeSelectedTables();
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        QSqlDriver* driver = db.driver();
        if (tables.isEmpty()) {
            return;
        }
        QStringList queries;
        foreach(const QString& table, tables) {
            QString query = QString("DROP TABLE %1").arg(driver->escapeIdentifier(table,QSqlDriver::TableName));
            queries << query;
        }
        onAppendQuery(connectionName,queries.join(";\n"));
    }

    if (result == update) {
        QString connectionName = this->connectionName();
        QStringList tables = schemaTreeSelectedTables();
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        QSqlDriver* driver = db.driver();
        if (tables.isEmpty()) {
            return;
        }
        QString table = tables.first();
        QSqlRecord record = db.record(table);
        QStringList names;
        QList<QVariant::Type> types;
        recordToNamesTypes(record,names,types);
        SelectColumnsDialog dialog;
        dialog.init(table, names, types);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        QString query = QString("UPDATE %1 SET %2 WHERE %3")
                .arg(driver->escapeIdentifier(table,QSqlDriver::TableName))
                .arg(dialog.dataChecked().join(" = , ") + " = ")
                .arg(dialog.keysChecked().join(" "));
        onAppendQuery(connectionName,query);
    }

    if (result == refresh) {
        QString connectionName = this->connectionName();
        updateTokens(connectionName);
        pushTokens(connectionName);
    }

}



QStringList MainWindow::schemaTreeSelectedTables() {
    QStringList tables;
    QModelIndexList indexes = ui->schemaTree->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return tables;
    }
    SchemaModel* model = qobject_cast<SchemaModel*>(ui->schemaTree->model());
    if (!model) {
        qDebug() << __FILE__ << __LINE__;
        return tables;
    }
    foreach(const QModelIndex& index, indexes) {
        if (model->isTable(index)) {
            tables << model->data(index).toString();
        }
    }
    return tables;
}



void MainWindow::on_schemaTree_doubleClicked(const QModelIndex &index)
{
    SchemaModel* model = qobject_cast<SchemaModel*>(ui->schemaTree->model());
    if (!model) {
        qDebug() << __FILE__ << __LINE__;
        return;
    }
    if (!model->isTable(index)) {
        return;
    }
    QString connectionName = this->connectionName();
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    bool mssql = db.driverName() == DRIVER_ODBC;
    QString table = model->data(index).toString();
    QString query = QString(mssql ? "SELECT TOP 100 * FROM %1" : "SELECT * FROM %1 LIMIT 100").arg(table).arg(table);
    onAppendQuery(connectionName, query);
}
