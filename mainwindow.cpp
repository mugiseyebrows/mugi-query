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

#include "sessionmodel.h"
#include "sessiontab.h"
#include "adddatabasedialog.h"
#include "removedatabasedialog.h"
#include "history.h"
#include "queryparser.h"
#include "queryhistorywidget.h"
#include "schemafetcher.h"
#include "setsplitersizesratio.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mQueryHistory(0)
{
    ui->setupUi(this);

    while (ui->sessionTabs->count() > 0) {
        ui->sessionTabs->removeTab(0);
    }

    mHistory = new History(this);

    SessionModel* m = new SessionModel(ui->sessionTree);
    ui->sessionTree->setModel(m);

    connect(m,
            SIGNAL(sessionAdded(QString,QString,QString)),
            this,
            SLOT(onSessionAdded(QString,QString,QString)));
    connect(m,SIGNAL(sessionRemoved(QString,QString)),this,SLOT(onSessionRemoved(QString,QString)));

    //connect(m,SIGNAL(database))

    /*
    m->addDatabase("foo");
    m->addSession(m->index(0,0));
    m->addDatabase("bar");
    m->addSession(m->index(1,0));
    m->addSession(m->index(1,0));
    */

    /*m->addDatabase("foo");
    m->addSession(m->index(0,0));
    m->addSession(m->index(0,0));
    m->removeRow(0,m->index(0,0));*/

    //m->addDatabase("_history");
    //updateCompleter("_history");

    connect(ui->sessionTree->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(onTreeCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->sessionTabs,SIGNAL(currentChanged(int)),this,SLOT(onTabsCurrentChanged(int)));

    QTimer::singleShot(0,this,SLOT(onAdjustSplitter()));
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

    QAction* addDatabase = cloneAction(ui->addDatabase,"&Add database",&menu);
    QAction* removeDatabase = cloneAction(ui->removeDatabase,"&Remove database",&menu);
    QAction* reconnect = ui->reconnect;
    QAction* addSession = cloneAction(ui->addSession,"Add &session",&menu);
    QAction* removeSession = cloneAction(ui->removeSession,"&Remove session",&menu);

    SessionModel* m = this->model();

    if (!index.isValid()) {

        menu.addAction(addDatabase);

    } else if (m->isDatabase(index)) {

        menu.addAction(addSession);
        menu.addSeparator();
        menu.addAction(addDatabase);
        menu.addAction(removeDatabase);
        menu.addAction(reconnect);

    } else if (m->isSession(index)) {

        menu.addAction(addSession);
        menu.addAction(removeSession);
        menu.addSeparator();
        menu.addAction(addDatabase);

    }

    menu.exec(QCursor::pos());

}

void MainWindow::onAdjustSplitter() {
    setSpliterSizesRatio(ui->splitter,1,4);
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

SessionTab *MainWindow::currentTab()
{
    int index = ui->sessionTabs->currentIndex();
    if (index < 0) {
        return 0;
    }
    return tab(index);
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

void MainWindow::onTabsCurrentChanged(int tabIndex) {
    //qDebug() << "onTabsCurrentChanged" << index;

    if (tabIndex < 0) {
        setWindowTitle("mugi-query");
        return;
    }

    QString connectionName = tab(tabIndex)->connectionName();
    QString name = tab(tabIndex)->name();
    QModelIndex index = model()->indexOf(connectionName,name);
    if (ui->sessionTree->currentIndex() != index) {
        ui->sessionTree->setCurrentIndex(index);
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QStringList title;
    title << db.driverName() << db.hostName() << db.userName() << db.databaseName();
    title = filterEmpty(title);
    setWindowTitle(title.join(" "));

}

void MainWindow::onSessionAdded(QString connectionName, QString name, QString namePrev) {

    int index;
    if (namePrev.isEmpty()) {
        index = -1;
    } else {
        index = tabIndex(ui->sessionTabs,namePrev);
    }

    qDebug() << "onSessionAdded" << connectionName << name << namePrev << index;

    SessionTab* tab = new SessionTab(connectionName, name, ui->sessionTabs);

    connect(tab,SIGNAL(showQueryHistory()),this,SLOT(onShowQueryHistory()));
    connect(tab,SIGNAL(addSessionWithQuery(QString)),this,SLOT(onAddSessionWithQuery(QString)));

    ui->sessionTabs->insertTab(index+1,tab,name);
    connect(tab,SIGNAL(query(QString)),this,SLOT(onQuery(QString)));

    tab->setCompleter(mCompleters.value(connectionName));

    ui->sessionTabs->setCurrentIndex(ui->sessionTabs->indexOf(tab));

    if (!mQuery.isEmpty()) {
        tab->setQuery(mQuery);
        mQuery = QString();
    }

}

void MainWindow::onAddSessionWithQuery(QString query) {
    mQuery = query;
    on_addSession_triggered();
}

void MainWindow::onCopyQuery(QString query) {
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->setQuery(query);
}

void MainWindow::onShowQueryHistory() {
    if (!mQueryHistory) {
        mQueryHistory = new QueryHistoryWidget();
        connect(mQueryHistory,SIGNAL(copyQuery(QString)),this,SLOT(onCopyQuery(QString)));
    }
    mQueryHistory->refresh();
    mQueryHistory->show();
    mQueryHistory->raise();
}

void MainWindow::onQuery(QString queries) {

    SessionTab* tab = currentTab();

    QString connectionName = tab->connectionName();

    QStringList queries_ = QueryParser::split(queries);

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

}

void MainWindow::updateCompleter(const QString& connectionName) {
    //ConnectionsModel* model = static_cast<ConnectionsModel*>(ui->connections->model());
    //qDebug() << "updateCompleter";
    QCompleter* completer = mCompleters.value(connectionName);

    if (!completer) {
        completer = new QCompleter();
        completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setWrapAround(true);
    }

    //qDebug() << "updateCompleter for connection" << connectionName;
    QStringList schema = SchemaFetcher::fetch(QSqlDatabase::database(connectionName));
    QStringListModel* stringListModel = new QStringListModel(schema,completer);

    completer->setModel(stringListModel);

    mCompleters[connectionName] = completer;
}

void MainWindow::addDatabase(bool showHistory)
{
    SessionModel* m = model();
    AddDatabaseDialog dialog(showHistory, this);
    QString connectionName;

    if (dialog.exec() == QDialog::Accepted) {
        connectionName = dialog.connectionName();
        m->addDatabase(connectionName);
    }
    mHistory->addDatabase(connectionName,dialog.driver(),dialog.host(),dialog.user(),dialog.password(),dialog.database(),dialog.port());

    if (mCompleters.contains(connectionName)) {
        mCompleters[connectionName]->deleteLater();
        mCompleters[connectionName] = 0;
    }

    updateCompleter(connectionName);

    ui->sessionTree->setCurrentIndex(m->index(m->rowCount()-1,0));
    on_addSession_triggered();
}

void MainWindow::on_addDatabase_triggered()
{
    addDatabase(false);
}

void MainWindow::onSessionRemoved(QString connectionName, QString name) {
    //qDebug() << "onSessionRemoved" << name;

    for(int i=0;i<ui->sessionTabs->count();i++) {
        SessionTab* tab = this->tab(i);
        if (tab->connectionName() == connectionName && tab->name() == name) {
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

void MainWindow::on_saveData_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->saveData();
}

void MainWindow::on_databaseHistory_triggered()
{
    addDatabase(true);
}

void MainWindow::on_queryHistory_triggered()
{
    onShowQueryHistory();
}

void MainWindow::on_addSession_triggered()
{
    SessionModel* m = model();
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!index.isValid()) {
        return;
    }
    m->addSession(index);
    ui->sessionTree->expandAll();
}

void MainWindow::on_removeSession_triggered()
{
    SessionModel* m = model();
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!m->isSession(index)) {
        return;
    }
    m->removeSession(index);
}

void MainWindow::on_removeDatabase_triggered()
{
    QModelIndex index = ui->sessionTree->currentIndex();
    if (!index.isValid()) {
        return;
    }

    SessionModel* m = model();
    QString connectionName = m->connectionName(index);
    RemoveDatabaseDialog dialog(connectionName,this);
    if (dialog.exec() == QDialog::Accepted) {
        m->removeDatabase(index);
    }
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
    event->accept();
}

void MainWindow::on_reconnect_triggered()
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
    urls["QMYSQL"] = "https://dev.mysql.com/doc/refman/8.0/en/sql-syntax-data-manipulation.html";
    urls["QODBC"] = "https://docs.microsoft.com/en-us/previous-versions/office/developer/office-2007/bb208930(v%3doffice.12)";
    urls["QSQLITE"] = "https://www.sqlite.org/lang.html";
    urls["QPSQL"] = "https://www.postgresql.org/docs/current/queries.html";
    if (urls.contains(driverName)) {
        QDesktopServices::openUrl(QUrl(urls[driverName]));
    }

}

void MainWindow::on_dataCopy_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->copySelected();
}

void MainWindow::on_dataSave_triggered()
{
    SessionTab* tab = currentTab();
    if (!tab) {
        return;
    }
    tab->saveData();
}
