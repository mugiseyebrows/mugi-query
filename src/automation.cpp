#include "automation.h"

#include <QTimer>
#include "mainwindow.h"
#include "databaseconnectdialog.h"
#include "databasehistorydialog.h"

#include <QApplication>
#include <QWindow>
#include <QDebug>
#include "sessiontab.h"
#include "xyplot.h"
#include "querymodelview.h"
#include "rowvaluesetter.h"
#include "xyplotmodel.h"
#include "distributionplotmodel.h"
#include "distributionplot.h"
#include "widget/datetimerangewidget.h"
#include "datetimerangewidgetmanager.h"
#include <QTextCodec>
#include "statview.h"
#include "clipboard.h"
#include <QDateTime>
#include <QFile>

namespace {

bool saveToFile(const QString& path, const QString& data) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "cannot open" << path;
        return false;
    }
    file.write(QTextCodec::codecForName("UTF-8")->fromUnicode(data));
    file.close();

    qDebug() << path << "saved";

    return true;
}

}

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

void Automation::connectToDatabaseFromHistory(const QString &connectionName)
{
    mQueued.enqueue(Action(Action::ActionConnectToDatabaseFromHistory, {connectionName}));
}

void Automation::query(const QString &connectionName, const QString &query) {
    mQueued.enqueue(Action(Action::ActionAppendQuery, {connectionName, query}));
    mQueued.enqueue(Action(Action::ActionExecuteCurrentQuery));
}

void Automation::showSaveDataDialog()
{
    mQueued.enqueue(Action(Action::ActionShowSaveDataDialog));
}

void Automation::setXYPlot(int row, const QString &x, const QString &y, const QString &line, const QString &marker)
{
    mQueued.enqueue(Action(Action::ActionSetXYPlot, {row, x, y, line, marker}));
}

void Automation::setDistributionPlot(int row, const QString &v, const QString &color)
{
    mQueued.enqueue(Action(Action::ActionSetDistributionPlot, {row, v, color}));
}

void Automation::setDistributionPlotManualRange(double vmin, double vmax)
{
    mQueued.enqueue(Action(Action::ActionSetDistributionPlotManualRange, {vmin, vmax}));
}

void Automation::showDistributionPlot()
{
    mQueued.enqueue(Action(Action::ActionShowDistributionPlot));
}

void Automation::showXYPlot()
{
    mQueued.enqueue(Action(Action::ActionShowXYPlot));
}

void Automation::showJoinHelper()
{
    mQueued.enqueue(Action(Action::ActionShowJoinHelper));
}

void Automation::showDataImportDialog()
{
    mQueued.enqueue(Action(Action::ActionShowDataImportDialog));
}

void Automation::showDateTimeRangeWidget()
{
    mQueued.enqueue(Action(Action::ActionShowDateTimeRangeWidget));
}

void Automation::showQueryHistory()
{
    mQueued.enqueue(Action(Action::ActionShowQueryHistory));
}

void Automation::showDatabaseHistory() 
{
    mQueued.enqueue(Action(Action::ActionShowDatabaseHistory));
}

void Automation::compareData()
{
    mQueued.enqueue(Action(Action::ActionCompareData));
}

void Automation::saveStat(const QString &path)
{
    mQueued.enqueue(Action(Action::ActionSaveStat,{path}));
}

void Automation::saveQuery(const QString &path)
{
    mQueued.enqueue(Action(Action::ActionSaveQuery,{path}));
}

void Automation::afterDialog(DatabaseConnectDialog *) {

}

void Automation::afterDialog(DatabaseHistoryDialog *) {

    if (mAction.type() == Action::ActionConnectToDatabaseFromHistory) {
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

    if (mQueued.isEmpty()) {
        mAction = Action();
        return;
    }

    mAction = mQueued.dequeue();

    qDebug() << "action" << mAction.type();

    if (mAction.type() == Action::ActionConnectToDatabaseFromHistory) {
        mainWindow()->databaseConnect(true);
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
    } else if (mAction.type() == Action::ActionSetDistributionPlotManualRange) {

        double vmin = mAction.arg(0).toDouble();
        double vmax = mAction.arg(1).toDouble();

        SessionTab* tab = mainWindow()->currentTab();
        QueryModelView* view = tab->tab(0);
        DistributionPlot* plot = view->distributionPlot();
        plot->setManualRange(vmin, vmax);

        next();
    } else if (mAction.type() == Action::ActionShowJoinHelper) {
        mainWindow()->on_queryJoin_triggered();
        next();
    } else if (mAction.type() == Action::ActionShowDistributionPlot ||
               mAction.type() == Action::ActionShowXYPlot) {
        SessionTab* tab = mainWindow()->currentTab();
        QueryModelView* view = tab->tab(0);
        if (mAction.type() == Action::ActionShowDistributionPlot) {
            view->showDistributionPlot();
        } else {
            view->showXYPlot();
        }
        next();
    } else if (mAction.type() == Action::ActionShowDataImportDialog) {
        mainWindow()->on_dataImport_triggered();
        next();
    } else if (mAction.type() == Action::ActionShowDateTimeRangeWidget) {

        DateTimeRangeWidget* widget = new DateTimeRangeWidget();
        DateTimeRangeWidgetManager* manager = new DateTimeRangeWidgetManager(widget);
        manager->init(widget);

        connect(widget,&DateTimeRangeWidget::dateTime1Changed,[=](QDateTime v){
            qDebug() << "dt1" << v;
        });
        connect(widget,&DateTimeRangeWidget::dateTime2Changed,[=](QDateTime v){
            qDebug() << "dt2" << v;
        });


        widget->show();
        next();
    } else if (mAction.type() == Action::ActionShowQueryHistory) {

        mainWindow()->onShowQueryHistory();
        next();

    } else if (mAction.type() == Action::ActionShowDatabaseHistory) {

        mainWindow()->onShowDatabaseDialog(true);
        
    } else if (mAction.type() == Action::ActionSaveStat) {

        QString path = mAction.arg(0).toString();
        SessionTab* tab = mainWindow()->currentTab();
        StatView* statView = tab->statView();
        QAbstractItemModel* model = statView->model();
        QItemSelection selection(model->index(0,0),model->index(model->rowCount()-1,model->columnCount()-1));
        QString error;
        QString text = Clipboard::selectedToString(model, selection, DataFormat::Csv, "\t", true, statView->locale(), error);
        saveToFile(path, text);
        next();

    } else if (mAction.type() == Action::ActionSaveQuery) {

        QString path = mAction.arg(0).toString();
        SessionTab* tab = mainWindow()->currentTab();
        QString query = tab->query();
        saveToFile(path, query);
        next();
    } else if (mAction.type() == Action::ActionCompareData) {

        mainWindow()->on_dataCompare_triggered();
        next();

    }
}



void Automation::beforeDialog(DatabaseConnectDialog *dialog)
{
    //qDebug() << "beforeDialog(AddDatabaseDialog *dialog)";
    mAddDatabaseDialog = dialog;
    QTimer::singleShot(0,this,SLOT(onAddDatabaseDialog()));
}

void Automation::beforeDialog(DatabaseHistoryDialog *dialog)
{
    //qDebug() << "beforeDialog(DatabaseHistoryDialog *dialog)";
    mDatabaseHistoryDialog = dialog;
    QTimer::singleShot(0,this,SLOT(onDatabaseHistoryDialog()));
}

void Automation::onAddDatabaseFromHistory()
{
    mainWindow()->databaseConnect(true);
}

void Automation::onAddDatabaseDialog() {

}

void Automation::onDatabaseHistoryDialog() {

    if (mAction.type() == Action::ActionConnectToDatabaseFromHistory) {
        QString connectionName = mAction.args().value(0).toString();
        mDatabaseHistoryDialog->select(connectionName);
    }
}

