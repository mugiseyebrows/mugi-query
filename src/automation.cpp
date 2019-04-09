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
#include "xyplot.h"
#include "querymodelview.h"
#include "rowvaluesetter.h"
#include "xyplotmodel.h"
#include "distributionplotmodel.h"
#include "distributionplot.h"

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

void Automation::showSaveDataDialog()
{
    mQueued.enqueue(Action(Action::ActionShowSaveDataDialog));
}

void Automation::setXYPlot(int row, const QString &x, const QString &y, const QString &line, const QString &marker)
{
    mQueued.enqueue(Action(Action::ActionSetXYPlot,vl(row,x,y,line,marker)));
}

void Automation::setDistributionPlot(int row, const QString &v, const QString &color)
{
    mQueued.enqueue(Action(Action::ActionSetDistributionPlot,vl(row,v,color)));
}

void Automation::showJoinHelper()
{
    mQueued.enqueue(Action(Action::ActionShowJoinHelper));
}

void Automation::afterDialog(AddDatabaseDialog *) {

}

void Automation::afterDialog(DatabaseHistoryDialog *) {

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
        } else if (mAction.type() == Action::ActionShowSaveDataDialog) {
            mainWindow()->on_dataSave_triggered();
            next();
        } else if (mAction.type() == Action::ActionSetXYPlot) {

            int row = mAction.arg(0).toInt();
            QString x = mAction.arg(1).toString();
            QString y = mAction.arg(2).toString();
            QString line = mAction.arg(3).toString();
            QString marker = mAction.arg(4).toString();

            SessionTab* tab = mainWindow()->currentTab();
            QueryModelView* view = tab->tab(0);
            XYPlot* plot = view->xyPlot();
            QAbstractItemModel* model = plot->tableModel();
            RowValueNotEmptySetter s(model,row);

            s(XYPlotModel::col_x,x);
            s(XYPlotModel::col_y,y);
            s(XYPlotModel::col_line,line);
            s(XYPlotModel::col_marker,marker);

            next();
        } else if (mAction.type() == Action::ActionSetDistributionPlot) {

            int row = mAction.arg(0).toInt();
            QString v = mAction.arg(1).toString();
            QString color = mAction.arg(2).toString();

            SessionTab* tab = mainWindow()->currentTab();
            QueryModelView* view = tab->tab(0);
            DistributionPlot* plot = view->distributionPlot();
            QAbstractItemModel* model = plot->tableModel();
            RowValueNotEmptySetter s(model,row);

            s(DistributionPlotModel::col_v,v);
            s(DistributionPlotModel::col_color,color);

            next();
        } else if (mAction.type() == Action::ActionShowJoinHelper) {
            mainWindow()->on_queryJoin_triggered();
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

