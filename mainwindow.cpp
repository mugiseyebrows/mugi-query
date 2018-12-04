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

#include "sessionmodel.h"
#include "sessiontab.h"
#include "adddatabasedialog.h"
#include "removedatabasedialog.h"
#include "history.h"
#include "queryparser.h"
#include "queryhistorywidget.h"
#include "schemafetcher.h"

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

    connect(ui->sessionTree->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(onTreeCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->sessionTabs,SIGNAL(currentChanged(int)),this,SLOT(onTabsCurrentChanged(int)));

    QTimer::singleShot(0,this,SLOT(onAdjustSplitter()));

    //connect(ui->sessionTree,SIGNAL()
}



void MainWindow::on_sessionTree_customContextMenuRequested(QPoint pos) {

    Q_UNUSED(pos);

    QModelIndex index = ui->sessionTree->currentIndex();

    QMenu menu(this);

    QAction* addDatabase = new QAction("add database",&menu);
    QAction* removeDatabase = new QAction("remove database",&menu);
    QAction* reconnect = new QAction("reconnect",&menu);
    QAction* addSession = new QAction("add session",&menu);
    QAction* removeSession = new QAction("remove session",&menu);

    QString connectionName;

    SessionModel* m = model();

    if (index.isValid()) {
        connectionName = m->connectionName(index);
    }

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

    QAction* action = menu.exec(QCursor::pos());

    if (action == addDatabase) {

        on_addDatabase_triggered();

    } else if (action == removeDatabase) {

        RemoveDatabaseDialog dialog(connectionName,this);
        if (dialog.exec() == QDialog::Accepted) {
            m->removeDatabase(index);
        }

    } else if (action == addSession) {

        m->addSession(index);

    } else if (action == removeSession) {

        m->removeSession(index);

    } else if (action == reconnect) {

        QSqlDatabase db = QSqlDatabase::database(connectionName);
        db.close();
        if (!db.open()) {
            QMessageBox::critical(this,"Error",db.lastError().text());
        }

    }

}



void MainWindow::onAdjustSplitter() {
    QList<int> sizes = ui->splitter->sizes();
    QList<int> sizes_;
    sizes_ << 200 << std::accumulate(sizes.begin(),sizes.end(),0) - 200;
    ui->splitter->setSizes(sizes_);

    ui->sessionTree->expandAll();

}

void MainWindow::onTreeCurrentChanged(QModelIndex index,QModelIndex) {
    //qDebug() << "onTreeCurrentChanged" << index;

    if (!index.isValid()) {
        return;
    }

    SessionModel* m = model();

    QString name;

    /*
    if (m->isSession(index)) {
        name = m->data(index).toString();
    } else if (m->isDatabase(index)) {
        if (m->rowCount(index) < 1) {
            return;
        }
        name = m->data(m->index(m->rowCount(index)-1,0,index)).toString();
    }
    */

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

    ui->sessionTabs->insertTab(index+1,tab,name);
    connect(tab,SIGNAL(query(QString)),this,SLOT(onQuery(QString)));

    tab->setCompleter(mCompleters.value(connectionName));
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
    qDebug() << "updateCompleter";
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

void MainWindow::on_addDatabase_triggered()
{
    SessionModel* m = model();
    AddDatabaseDialog dialog(this);
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

SessionModel *MainWindow::model()
{
    return qobject_cast<SessionModel*>(ui->sessionTree->model());
}
