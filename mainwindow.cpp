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
#include "adddatabasedialog.h"
#include "removedatabasedialog.h"
#include "history.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

#include <QCursor>

void MainWindow::on_sessionTree_customContextMenuRequested(QPoint pos) {

    Q_UNUSED(pos);

    QModelIndex index = ui->sessionTree->currentIndex();

    QMenu menu(this);

    QAction* addDatabase = new QAction("add database",&menu);
    QAction* removeDatabase = new QAction("remove database",&menu);
    QAction* addSession = new QAction("add session",&menu);
    QAction* removeSession = new QAction("remove session",&menu);

    SessionModel* m = model();

    if (!index.isValid()) {

        menu.addAction(addDatabase);

    } else if (m->isDatabase(index)) {

        menu.addAction(addSession);
        menu.addSeparator();
        menu.addAction(addDatabase);
        menu.addAction(removeDatabase);

    } else if (m->isSession(index)) {

        menu.addAction(addSession);
        menu.addAction(removeSession);
        menu.addSeparator();
        menu.addAction(addDatabase);

    }

    //qDebug() << pos << ui->sessionTabs->mapToParent(pos);

    QAction* action = menu.exec(QCursor::pos());

    if (action == addDatabase) {

        on_addDatabase_triggered();

    } else if (action == removeDatabase) {

        QString name = m->databaseName(index);

        RemoveDatabaseDialog dialog(name,this);
        if (dialog.exec() == QDialog::Accepted) {
            m->removeDatabase(index);
        }


    } else if (action == addSession) {

        m->addSession(index);

    } else if (action == removeSession) {

        m->removeSession(index);

    }

}

#include <QStandardPaths>

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

    if (tabIndex < 0) {
        return;
    }

    QString connectionName = tab(tabIndex)->connectionName();
    QString name = tab(tabIndex)->name();
    QModelIndex index = model()->indexOf(connectionName,name);
    if (ui->sessionTree->currentIndex() != index) {
        ui->sessionTree->setCurrentIndex(index);
    }
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
    ui->sessionTabs->insertTab(index+1,tab,name);
    connect(tab,SIGNAL(query(QString)),this,SLOT(onQuery(QString)));
}

void MainWindow::onQuery(QString query) {
    qDebug() << query;
    mHistory->addQuery(currentTab()->connectionName(),query);
}

void MainWindow::on_addDatabase_triggered()
{
    SessionModel* m = model();
    AddDatabaseDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m->addDatabase(dialog.connectionName());
    }
    mHistory->addDatabase(dialog.connectionName(),dialog.driver(),dialog.host(),dialog.user(),dialog.password(),dialog.database(),dialog.port());
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
