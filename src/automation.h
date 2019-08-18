#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <QObject>
#include <QQueue>
#include <QVariantList>
class MainWindow;
class DatabaseConnectDialog;
class DatabaseHistoryDialog;

#include "action.h"

class Automation : public QObject
{
    Q_OBJECT
public:

    static Automation* mInstance;
    static Automation* instance(QObject *parent = 0);

    void connectToDatabaseFromHistory(const QString& connectionName);
    void query(const QString &connectionName, const QString &query);
    void showSaveDataDialog();
    void setXYPlot(int row, const QString& x, const QString& y, const QString& line, const QString& marker);
    void setDistributionPlot(int row, const QString& v, const QString& color);
    void showDistributionPlot();
    void showXYPlot();
    void showJoinHelper();
    void showDataImportDialog();
    void showDateTimeRangeWidget();

    void beforeDialog(DatabaseHistoryDialog* dialog);
    void beforeDialog(DatabaseConnectDialog *dialog);

    void afterDialog(DatabaseConnectDialog *dialog);
    void afterDialog(DatabaseHistoryDialog *dialog);

    void start();

    void next();
protected:

    QQueue<Action> mQueued;
    Action mAction;

    MainWindow* mainWindow();

    Automation(QObject* parent = 0);

    DatabaseConnectDialog* mAddDatabaseDialog;
    DatabaseHistoryDialog* mDatabaseHistoryDialog;

protected slots:

    void onAddDatabaseFromHistory();
    void onAddDatabaseDialog();
    void onDatabaseHistoryDialog();
    void onStart();
};

#endif // AUTOMATION_H
