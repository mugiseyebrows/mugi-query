#include "automation.h"

#include <QTimer>
#include "mainwindow.h"
#include "adddatabasedialog.h"
#include "databasehistorydialog.h"

#include <QApplication>
#include <QWindow>
#include <QDebug>
#include "lit.h"
#include "sessiontab.h"

using namespace Lit;

Automation *Automation::mInstance = 0;

Automation *Automation::instance(QObject *parent)
{
    if (!mInstance) {
        mInstance = new Automation(parent);
    }
    return mInstance;
}

MainWindow *Automation::mainWindow()
{
    MainWindow* window = qobject_cast<MainWindow*>(parent());

    return window;
}

Automation::Automation(QObject *parent) : QObject(parent), mAddDatabaseDialog(0), mDatabaseHistoryDialog(0)
{

}

void Automation::addDatabaseFromHistory(const QString &connectionName)
{
    mQueued.enqueue(Action(Action::ActionAddDatabaseFromHistory, vl(connectionName)));
}

void Automation::query(const QString &connectionName, const QString &query) {
    mQueued.enqueue(Action(Action::ActionAppendQuery, vl(connectionName,query)));
    mQueued.enqueue(Action(Action::ActionExecuteCurrentQuery));
}

void Automation::afterDialog(AddDatabaseDialog *dialog) {

}

void Automation::afterDialog(DatabaseHistoryDialog *dialog) {

    if (mAction.type() == Action::ActionAddDatabaseFromHistory) {
        mAddDatabaseDialog->accept();
        next();
    }
}

void Automation::start()
{
    QTimer::singleShot(0,this,SLOT(onStart()));
}

void Automation::next() {
    start();
}



void Automation::onStart() {
    if (!mQueued.isEmpty()) {
        mAction = mQueued.dequeue();
        if (mAction.type() == Action::ActionAddDatabaseFromHistory) {
            mainWindow()->addDatabase(true);
        } else if (mAction.type() == Action::ActionAppendQuery) {
            QString connectionName = mAction.arg(0).toString();
            QString query = mAction.arg(1).toString();
            mainWindow()->onAppendQuery(connectionName,query);
            next();
        } else if (mAction.type() == Action::ActionExecuteCurrentQuery) {
            mainWindow()->currentTab()->on_execute_clicked();
            next();
        }
    } else {
        mAction = Action();
    }
}

void Automation::beforeDialog(AddDatabaseDialog *dialog)
{
    qDebug() << "beforeDialog(AddDatabaseDialog *dialog)";
    mAddDatabaseDialog = dialog;
    QTimer::singleShot(0,this,SLOT(onAddDatabaseDialog()));
}

void Automation::beforeDialog(DatabaseHistoryDialog *dialog)
{
    qDebug() << "beforeDialog(DatabaseHistoryDialog *dialog)";
    mDatabaseHistoryDialog = dialog;
    QTimer::singleShot(0,this,SLOT(onDatabaseHistoryDialog()));
}

void Automation::onAddDatabaseFromHistory()
{
    mainWindow()->addDatabase(true);
}

void Automation::onAddDatabaseDialog() {

}

void Automation::onDatabaseHistoryDialog() {

    if (mAction.type() == Action::ActionAddDatabaseFromHistory) {
        QString connectionName = mAction.args().value(0).toString();
        mDatabaseHistoryDialog->select(connectionName);
    }
}

