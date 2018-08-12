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

#include "sessionmodel.h"
#include "sessiontab.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    while (ui->sessionTabs->count() > 0) {
        ui->sessionTabs->removeTab(0);
    }

    initHistory();

    SessionModel* m = new SessionModel(ui->sessionTree);
    ui->sessionTree->setModel(m);

    connect(m,
            SIGNAL(sessionAdded(QString,QString,QString)),
            this,
            SLOT(onSessionAdded(QString,QString,QString)));
    connect(m,SIGNAL(sessionRemoved(QString)),this,SLOT(onSessionRemoved(QString)));

    m->addDatabase("foo");
    m->addSession(m->index(0,0));
    m->addDatabase("bar");
    m->addSession(m->index(1,0));
    m->addSession(m->index(1,0));

    connect(ui->sessionTree->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(onTreeCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->sessionTabs,SIGNAL(currentChanged(int)),this,SLOT(onTabsCurrentChanged(int)));

    QTimer::singleShot(0,this,SLOT(onAdjustSplitter()));
}



void MainWindow::initHistory() {

    QString home = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    QDir d(QDir(home).filePath(".mugi-query"));
    if (!d.exists()) {
        d.mkdir(".");
    }
    mHistory = QSqlDatabase::addDatabase("QSQLITE","_history");
    mHistory.setDatabaseName(d.filePath("history.sqlite"));
    if (!mHistory.open()) {
        QMessageBox::critical(this,"error",mHistory.lastError().text());
        return;
    }
    mHistory.exec("create table if not exists query(database varchar(64),date datetime,value text)");
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
    return tab(ui->sessionTabs->currentIndex());
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

    QString name = tab(tabIndex)->name();
    QString database = tab(tabIndex)->database();
    QModelIndex index = model()->indexOf(database,name);
    if (ui->sessionTree->currentIndex() != index) {
        ui->sessionTree->setCurrentIndex(index);
    }
}


void MainWindow::onSessionAdded(QString database, QString name, QString namePrev) {
    qDebug() << "onSessionAdded" << database << name << namePrev;

    int index;
    if (namePrev.isEmpty()) {
        index = -1;
    } else {
        index = tabIndex(ui->sessionTabs,namePrev);
    }

    SessionTab* tab = new SessionTab(database, name, ui->sessionTabs);
    ui->sessionTabs->insertTab(index+1,tab,name);
    connect(tab,SIGNAL(query(QString)),this,SLOT(onQuery(QString)));
}

void MainWindow::onQuery(QString query) {

    qDebug() << query;
    addToHistory(currentTab()->database(),query);
}



void MainWindow::addToHistory(const QString& database, const QString& query) {

    QSqlQuery q(mHistory);
    q.prepare("insert into query(database,date,value) values(?,datetime('now'),?)");
    q.addBindValue(database);
    q.addBindValue(query);
    q.exec();
}

void MainWindow::onSessionRemoved(QString name) {
    qDebug() << "onSessionRemoved" << name;
}

MainWindow::~MainWindow()
{
    delete ui;
}

SessionModel *MainWindow::model()
{
    return qobject_cast<SessionModel*>(ui->sessionTree->model());
}
