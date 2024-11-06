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
#include <QSqlTableModel>
#include <QTableView>

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
#include "widget/datacomparewidget.h"

#include <QThread>
#include <QSqlRecord>
#include <QSqlField>
#include "datautils.h"
#include "toolmysqldialog.h"
#include <QProcess>
#include "settings.h"
#include <QFileDialog>
#include "automate.h"
#include "showandraise.h"
#include "schema2tablesmodel.h"
#include "schema2tablemodel.h"
#include "tolower.h"
#include "schema2data.h"
#include "schema2view.h"
#include "schema2treemodel.h"
#include "schema2treeproxymodel.h"
#include "codewidget.h"
#include "settingsdirectorydialog.h"
#include <sqlparse.h>

using namespace DataUtils;

namespace {



void recordToNamesTypes(const QSqlRecord& record, QStringList& names, QList<QMetaType::Type>& types) {
    for(int c=0;c<record.count();c++) {
        names << record.fieldName(c);
        types << (QMetaType::Type) record.field(c).metaType().id();
    }
}

QStringList repeat(const QString& item, int n) {
    QStringList res;
    for(int i=0;i<n;i++) {
        res.append(item);
    }
    return res;
}

}

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

    SessionModel* sessionModel = new SessionModel(ui->sessionTree);
    ui->sessionTree->setModel(sessionModel);

    //mSchemaModel = new SchemaModel(this);

#if 0
    ui->schemaTree->setModel(new SchemaModel(ui->schemaTree));
#endif

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

    if (qApp->applicationDirPath().toLower().endsWith("debug")) {
        automate(this);
    }
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
    SplitterUtil::setRatio(ui->horizontalSplitter,{1,3});
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
    //mSchemaModel->update(mTokens);
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

    /*SchemaModel* schemaModel = this->schemaModel();
    QModelIndex rootIndex = schemaModel->find(connectionName);
    if (rootIndex != ui->schemaTree->rootIndex()) {
        ui->schemaTree->setRootIndex(rootIndex);
    }*/


    updateSchemaTreeModel();

}

void MainWindow::updateSchemaTreeModel() {

    int index = ui->sessionTabs->currentIndex();

    Schema2TreeProxyModel* treeProxy;

    if (index < 0) {
        treeProxy = nullptr;
    } else {
        QString connectionName = tab(index)->connectionName();
        Schema2Data* data = Schema2Data::instance(connectionName, this);
        treeProxy = data->tables()->treeProxy();
    }

    if (ui->schemaTree->model() != treeProxy) {
        qDebug() << "ui->schemaTree->model() != tree";
        ui->schemaTree->setModel(treeProxy);
        ui->tableName->setText("");
    }

}

void MainWindow::expandSchemaTree() {
    ui->schemaTree->expandAll();
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
    showAndRaise(this);
}

void MainWindow::onShowQueryHistory() {
    if (!mQueryHistory) {
        mQueryHistory = new QueryHistoryWidget();
        connect(mQueryHistory,SIGNAL(appendQuery(QString,QString)),
                this,SLOT(onAppendQuery(QString,QString)));
    }
    showAndRaise(mQueryHistory);
    auto* tab = currentTab();
    if (tab == nullptr) {
        return;
    }
    mQueryHistory->refresh(tab->connectionName());
}

QStringList filterBlank(const QStringList items) {
    QStringList res;
    foreach(QString item,items) {
        if (!item.trimmed().isEmpty()) {
            res.append(item);
        }
    }
    return res;
}



void MainWindow::onQuery(QString queries) {

    SessionTab* tab = currentTab();

    QString connectionName = tab->connectionName();

    // todo remove comments for drivers that do not support comments
    QStringList queries_ = filterBlank(SqlParse::splitQueries(queries));

    QStringList errors;
    QList<QSqlQueryModel*> models;
    QList<int> perf;
    QList<int> rowsAffected;

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QElapsedTimer time;
    time.start();

    QList<QueryEffect> effects;

    foreach(QString query, queries_) {

        History::instance()->addQuery(connectionName,query.trimmed());

        auto effect = SqlParse::queryEffect(query);
        if (!effect.isNone()) {
            effects.append(effect);
        }

        QSqlQuery q(db);
        QSqlQueryModel* model = 0;
        QString error;
        int rowsAffected_ = -1;
        if (!q.exec(query)) {
            error = q.lastError().text();
        } else {
            rowsAffected_ = q.numRowsAffected();
            if (q.isSelect()) {
                model = new QSqlQueryModel();
                model->setQuery(std::move(q));
            }
        }
        perf << time.restart();
        models << model;
        errors << error;
        rowsAffected << rowsAffected_;
    }

    tab->setResult(queries_,errors,models,perf,rowsAffected);

    if (effects.size() > 0) {
        updateTokens(connectionName);
        pushTokens(connectionName);
        // todo optimize
        Schema2Data* data = Schema2Data::instance(connectionName, this);
        data->pull();
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

void MainWindow::onShowDatabaseDialog(bool showHistory) {
    DatabaseConnectDialog dialog(showHistory, this);
    dialog.exec();
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
    History::instance()->addDatabase(connectionName,dialog.driver(),dialog.host(),dialog.user(),dialog.password(),dialog.database(),dialog.port());
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

    updateSchemaTreeModel();
}

QString MainWindow::connectionName() const {
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!index.isValid()) {
        return QString();
    }
    SessionModel* m = model();
    return m->connectionName(index);
}

QSqlDatabase MainWindow::database() const
{
    QString connectionName = this->connectionName();
    if (connectionName.isEmpty()) {
        return QSqlDatabase();
    }
    return QSqlDatabase::database(connectionName);
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
    copySelected(CopyFormat::Table);
}

void MainWindow::on_selectionCopyAsList_triggered()
{
    copySelected(CopyFormat::List);
}

void MainWindow::on_selectionCopyAsCondition_triggered()
{
    copySelected(CopyFormat::Condition);
}

void MainWindow::on_selectionCopyNames_triggered() {
    copySelected(CopyFormat::Names);
}

void MainWindow::copySelected(CopyFormat fmt)
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->copySelected(fmt);
}

void MainWindow::on_dataSave_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->saveData();
}

#include "datacomparewidget2.h"

void MainWindow::on_dataCompare_triggered()
{
#if 0
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->fetchAll();
    QSqlQueryModel* model = tab->currentModel();
    if (!model) {
        return;
    }
    mCompareModels.append(model);
    if (mCompareModels.size() > 1) {
        DataCompareWidget* widget = new DataCompareWidget();
        widget->setModels(mCompareModels[0], mCompareModels[1]);
        widget->show();
        mCompareModels.clear();
    }
#endif
}

#include "tools.h"

void MainWindow::on_toolsMysql_triggered()
{
    QSqlDatabase db = database();
    QString connectionName = this->connectionName();
    if (!db.isValid() || !db.isOpen() || db.driverName() != DRIVER_MYSQL) {
        return;
    }
    Tools::mysql(db, this);
    Schema2Data* data = Schema2Data::instance(connectionName, this);
    data->pull();
}

void MainWindow::on_toolsMysqldump_triggered()
{
    QSqlDatabase db = database();
    if (!db.isValid() || !db.isOpen() || db.driverName() != DRIVER_MYSQL) {
        return;
    }
    Tools::mysqldump(db, this);
}

#include <QClipboard>
#if 0
void MainWindow::on_codePython_triggered()
{
    // todo parse and transpile queries


    QSqlDatabase db = database();

    QString databaseName = db.databaseName();
    QString userName = db.userName();
    QString password = db.password();
    QString hostName = db.hostName();

    QString connectionName = this->connectionName();

    if (db.driverName() == DRIVER_MYSQL) {
        QString code = QString("import mysql.connector\n"
                "%1 = mysql.connector.connect(host=\"%2\",user=\"%3\",password=\"%4\",database=\"%5\")")
                .arg(connectionName)
                .arg(hostName)
                .arg(userName)
                .arg(password)
                .arg(databaseName);
        qApp->clipboard()->setText(code);
        QMessageBox::information(this, "Success", "Text copied into clipboard");
    } else {
        QMessageBox::critical(this, "Error", QString("Not implemented for driver %1").arg(db.driverName()));
    }

}
#endif

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
    showAndRaise(mJoinHelpers);
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

    QAction* insert = new QAction("Insert",&menu);
    menu.addAction(insert);

    QAction* join = new QAction("Join",&menu);
    menu.addAction(join);

    QAction* update = new QAction("Update",&menu);
    menu.addAction(update);

    QAction* refresh = new QAction("Refresh",&menu);
    menu.addAction(refresh);

    QAction* drop = new QAction("Drop",&menu);
    menu.addAction(drop);

    QAction* edit = new QAction("Edit",&menu);
    menu.addAction(edit);


    QAction* result = menu.exec(QCursor::pos());

    qDebug() << "result" << result;

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
#if 0
        QStringList queries;
        foreach(const QString& table, tables) {
            QString query = QString("DROP TABLE %1").arg(driver->escapeIdentifier(table,QSqlDriver::TableName));
            queries << query;
        }
        onAppendQuery(connectionName,queries.join(";\n"));
#endif

        //Schema2TreeModel* schemaTreeModel = this->schemaTreeModel();
        QString question = QString("Are you sure you want to drop %1").arg(tables.size() == 1 ? QString(tables[0]) : tables.join(" and "));
        auto answer = QMessageBox::question(this, QString(), question, QMessageBox::Yes | QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return;
        }

        STablesDiff diff;

        QStringList errors;
        QStringList failed;
        for(const QString& table: tables) {
            QString query = QString("DROP TABLE %1").arg(driver->escapeIdentifier(table,QSqlDriver::TableName));
            QSqlQuery q(db);
            if (q.exec(query)) {
                diff.dropped.append(table);
            } else {
                QMessageBox::critical(this, QString(), q.lastError().text());
                failed.append(table);
                errors.append(q.lastError().text());
                break;
            }
        }

        if (!diff.isEmpty()) {
            Schema2Data* data = Schema2Data::instance(connectionName, this);
            data->tables()->merge(diff);
        }

        if (errors.size() > 0) {
            QString message = QString("Failed to drop %1\nerror(s):\n%2")
                    .arg(failed.size() == 1 ? QString(failed[0]) : failed.join(" and "))
                    .arg(errors.join("\n"));
            QMessageBox::critical(this, QString(), message);
        }
    }

    if (result == update || result == insert) {
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
        QList<QMetaType::Type> types;
        recordToNamesTypes(record,names,types);
        SelectColumnsDialog dialog;

        if (result == update) {
            dialog.initUpdate(table, names, types);
        } else if (result == insert) {
            dialog.initInsert(table, names, types);
        }

        if (dialog.exec() != QDialog::Accepted) {
            return;
        }

        QString query;
        if (result == update) {

            auto dataChecked = dialog.dataChecked();
            auto keysChecked = dialog.keysChecked();

            if (dataChecked.isEmpty()) {
                return;
            }

            query = QString("UPDATE %1 SET %2 WHERE %3")
                    .arg(driver->escapeIdentifier(table,QSqlDriver::TableName))
                    .arg(dataChecked.join(" = , ") + " = ")
                    .arg(keysChecked.join(" "));

        } else if (result == insert) {

            auto checked = dialog.checked();

            if (checked.isEmpty()) {
                return;
            }

            query = QString("INSERT INTO %1(%2) VALUES (%3)")
                    .arg(driver->escapeIdentifier(table,QSqlDriver::TableName))
                    .arg(checked.join(", "))
                    .arg(repeat("", checked.size()).join(", "));

        }
        onAppendQuery(connectionName,query);
    }

    if (result == refresh) {
        QString connectionName = this->connectionName();
        Schema2Data* data = Schema2Data::instance(connectionName, this);
        data->pull();
    }

    if (result == edit) {
        QString connectionName = this->connectionName();
        QStringList tables = schemaTreeSelectedTables();
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        for(const QString& table: std::as_const(tables)) {
            QTableView* view = new QTableView();
            view->setAttribute(Qt::WA_DeleteOnClose);
            QSqlTableModel* model = new QSqlTableModel(view, db);
            model->setEditStrategy(QSqlTableModel::OnFieldChange);
            model->setTable(table);
            model->select();
            view->setModel(model);
            view->show();
        }
    }

}


Schema2TreeProxyModel* MainWindow::schemaTreeProxyModel() const {
    auto* model = ui->schemaTree->model();
    if (!model) {
        return nullptr;
    }
    return qobject_cast<Schema2TreeProxyModel*>(model);
}

Schema2TreeModel* MainWindow::schemaTreeModel() const {
    Schema2TreeProxyModel* treeProxyModel = schemaTreeProxyModel();
    if (!treeProxyModel) {
        qDebug() << "!proxyModel" << __FILE__ << __LINE__;
        return nullptr;
    }
    return qobject_cast<Schema2TreeModel*>(treeProxyModel->sourceModel());
}

QStringList MainWindow::schemaTreeSelectedTables() {

    QStringList tables;

    Schema2TreeProxyModel* proxyModel = this->schemaTreeProxyModel();
    Schema2TreeModel* model = this->schemaTreeModel();

    auto indexes = ui->schemaTree->selectionModel()->selectedIndexes();
    for(const QModelIndex& index: indexes) {
        QModelIndex sourceIndex = proxyModel->mapToSource(index);
        if (model->isTable(sourceIndex)) {
            tables.append(model->tableName(sourceIndex));
        } else {
            qDebug() << "not a table" << sourceIndex;
        }
    }

#if 0
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
#endif
    return tables;
}



void MainWindow::on_schemaTree_doubleClicked(const QModelIndex &index)
{
#if 0
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
#endif
}


void MainWindow::on_schemaEdit_triggered()
{
    QString connectionName = this->connectionName();
    if (connectionName.isEmpty()) {
        return;
    }
    Schema2Data* data = Schema2Data::instance(connectionName, this);
    Schema2View* view = data->view();
    showAndRaise(view);
}

#include "xjoinwidget.h"

void MainWindow::on_toolsJoin_triggered()
{
    XJoinWidget* widget = new XJoinWidget();
    widget->setAttribute(Qt::WA_DeleteOnClose);

    QStringList connectionNames = model()->connectionNames();

    widget->init(connectionNames);

    widget->show();

}



static bool containsAll(const QStringList& ordered, const QStringList& related, const QString& name) {
    for(const QString& item: related) {
        if (item.toLower() == name.toLower()) {
            continue;
        }
        if (!toLower(ordered).contains(item.toLower())) {
            return false;
        }
    }
    return true;
}

void MainWindow::on_codeCopyOrder_triggered()
{
    QString connectionName = this->connectionName();

    //QStringList names = QSqlDatabase::database(connectionName).tables();

    // todo case correctness
    Schema2Data* data = Schema2Data::instance(connectionName, this);
    auto* tables = data->tables();
    QStringList names = tables->tableNames();
    QHash<QString, QStringList> relations;
    for(const QString& name: names) {
        relations[name] = tables->table(name)->relatedTables();
    }
    QStringList ordered;

    int i = 0;
    while(i < names.size()) {
        auto name = names[i];
        if (relations[name].isEmpty()) {
            ordered.append(name);
            names.removeAt(i);
            i--;
        }
        i++;
    }
    while(!names.isEmpty()) {
        int count1 = names.size();
        i = 0;
        while(i < names.size()) {
            auto name = names[i];
            auto related = relations[name];
            if (containsAll(ordered, related, name)) {
                ordered.append(name);
                names.removeAt(i);
                i--;
            }
            i++;
        }
        int count2 = names.size();
        if (count1 == count2) {
            qDebug() << "count1 == count2" << __FILE__ << __LINE__;



            return;
        }
    }

    qApp->clipboard()->setText(ordered.join("\n") + "\n");
    QMessageBox::information(this, "", "Copied to clipboard");
}

#include "comparetablewidget.h"

void MainWindow::on_dataCompareTable_triggered()
{
    CompareTableWidget* widget = new CompareTableWidget();
    showAndRaise(widget);
}


void MainWindow::on_dataCompareDatabase_triggered()
{

}

void MainWindow::on_dataStatistics_triggered()
{
    QString connectionName = this->connectionName();
    Schema2Data* data = Schema2Data::instance(connectionName, this);
    data->showDataStatistics(this);
}


void MainWindow::on_codeRelations_triggered()
{
    QString connectionName = this->connectionName();
    Schema2Data* data = Schema2Data::instance(connectionName, this);
    data->copyRelationsToClipboard(this);
}


void MainWindow::on_codePrimaryKeys_triggered()
{
    QString connectionName = this->connectionName();
    Schema2Data* data = Schema2Data::instance(connectionName, this);
    data->copyPrimaryKeysToClipboard(this);
}

#include "datatruncatedialog.h"
#include <QTableView>
#include <QProgressDialog>

void MainWindow::on_dataTruncate_triggered()
{
    QSqlDatabase db = database();
    if (!db.isValid() || db.driverName() != DRIVER_MYSQL) {
        return;
    }
    DataTruncateDialog dialog(db, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    QStringList tables = dialog.tables();
    QStringList errors;

    QProgressDialog progress(this);
    progress.show();
    progress.setMaximum(tables.size());
    qApp->processEvents();

    int complete = 0;

    for(const QString& table: tables) {
        QSqlQuery q(db);
        if (!q.exec(QString("truncate `%1`").arg(table))) {
            //errors.append(q.lastError().text());
            if (!q.exec(QString("delete from `%1` where 1").arg(table))) {
                errors.append(q.lastError().text());
            }
        }
        complete += 1;
        progress.setValue(complete);
        qApp->processEvents();
    }

    if (!errors.isEmpty()) {
        QTableView* view = new QTableView();

        QTimer::singleShot(0,[=](){
            QRect rect = view->geometry();
            rect.setSize(QSize(600,500));
            view->setGeometry(rect);
        });

        QStringListModel* model = new QStringListModel(errors, view);
        view->setModel(model);
        view->horizontalHeader()->setStretchLastSection(true);
        view->verticalHeader()->setDefaultSectionSize(100);
        view->horizontalHeader()->setVisible(false);
        view->setWindowTitle("Errors");
        view->show();
    } else {
        QMessageBox::information(this, "Success", QString("Succesfully truncated %1 tables").arg(tables.size()));
    }

}



void MainWindow::on_settingsDirectory_triggered()
{
    SettingsDirectoryDialog dialog;
    dialog.exec();
}

void MainWindow::on_tableName_textChanged(const QString &text)
{
    auto* model = ui->schemaTree->model();
    if (!model) {
        return;
    }
    auto* proxyModel = qobject_cast<Schema2TreeProxyModel*>(model);
    if (!proxyModel) {
        qDebug() << "!proxyModel" << __FILE__ << __LINE__;
        return;
    }
    QRegularExpression regExp(text, QRegularExpression::CaseInsensitiveOption);
    if (regExp.isValid()) {
        proxyModel->setFilterRegularExpression(regExp);
    }
}

void MainWindow::on_selectionViewAsHex_triggered()
{
    SessionTab* tab = currentTab();
    tab->viewAsHex();
}


void MainWindow::on_selectionViewAsString_triggered()
{
    SessionTab* tab = currentTab();
    tab->viewAsString();
}

void MainWindow::on_codeMysqlConnector_triggered()
{
    QSqlDatabase db = database();

    QString databaseName = db.databaseName();
    QString userName = db.userName();
    QString password = db.password();
    QString hostName = db.hostName();

    QString connectionName = this->connectionName();

    if (db.driverName() == DRIVER_MYSQL) {
        QString code = QString("# pip install mysql-connector-python\n"
                               "\n"
                               "import mysqlconnector\n"
                               "%1 = mysql.connector.connect(host=\"%2\",user=\"%3\",password=\"%4\",database=\"%5\")")
                .arg(connectionName)
                .arg(hostName)
                .arg(userName)
                .arg(password)
                .arg(databaseName);
        qApp->clipboard()->setText(code);

        CodeWidget* widget = new CodeWidget();
        widget->setText(code + "\n");
        widget->show();

    } else {
        QMessageBox::critical(this, "Error", QString("Not implemented for driver %1").arg(db.driverName()));
    }
}

static QString replace(const QString& s, const QString& a, const QString& b) {
    QString s_ = s;
    s_.replace(a, b);
    return s_;
}

static QString replace(const QString& s, const QRegularExpression& a, const QString& b) {
    QString s_ = s;
    s_.replace(a, b);
    return s_;
}

static QString pyvar(const QString& name) {
    // todo remove spaces and translit
    QString v = replace(name, QRegularExpression("[.-]"), "_");
    if (v.size() > 0 && v[0].isDigit()) {
        v = "db" + v;
    }
    return v;
}


static QString pipInstall(const QStringList& items) {
    return "# pip install " + items.join(" ") + "\n\n";
}

static QString sqlAlchemyCreateEngine(const QSqlDatabase& db, const QString& engineName) {

    QString code = QString("%5 = create_engine(\"mysql+mysql.connector://%1:%2@%3:3306/%4\", echo=True)\n")
            .arg(db.userName())
            .arg(db.password())
            .arg(db.hostName())
            .arg(db.databaseName())
            .arg(engineName);
    return code;
}

void MainWindow::on_codeSqlAlchemy_triggered()
{
    QSqlDatabase db = database();

    if (db.driverName() == DRIVER_MYSQL) {

        QString engineName = pyvar(connectionName());

        QString code = pipInstall({"mysql-connector-python", "SQLAlchemy"})
                + "from sqlalchemy import create_engine\n"
                + sqlAlchemyCreateEngine(database(), engineName);

        CodeWidget* widget = new CodeWidget();
        widget->setText(code + "\n");
        widget->show();

    } else {
        QMessageBox::critical(this, "Error", QString("Not implemented for driver %1").arg(db.driverName()));
    }

}

void MainWindow::on_codePandas_triggered()
{
    QSqlDatabase db = database();

    if (db.driverName() == DRIVER_MYSQL) {

        QStringList queries;
        auto* tab = currentTab();
        if (tab) {
            queries = filterBlank(SqlParse::splitQueries(tab->query()));
        }

        QString engineName = pyvar(connectionName());

        QStringList frames;

        for(const QString& query: queries) {
            QString tableName;
            QString expr;
            if (SqlParse::isSimpleSelect(query, tableName)) {
                expr = "'" + tableName + "'";
            } else {
                expr = query.trimmed();
                if (expr.indexOf("\n") > -1) {
                    expr = "\"\"\"" + expr + "\"\"\"\n";
                } else {
                    expr = "'" + expr + "'";
                }
            }
            frames.append(QString("df%1 = pd.read_sql(%2, %3)").arg(frames.size() + 1).arg(expr).arg(engineName));
        }

        QString code = pipInstall({"mysql-connector-python", "SQLAlchemy", "pandas"})
                + "import pandas as pd\n"
                + "from sqlalchemy import create_engine\n"
                + sqlAlchemyCreateEngine(database(), engineName)
                + frames.join("\n\n");

        CodeWidget* widget = new CodeWidget();
        widget->setText(code + "\n");
        widget->show();

    } else {
        QMessageBox::critical(this, "Error", QString("Not implemented for driver %1").arg(db.driverName()));
    }



}

