#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <QObject>
#include <QQueue>
#include <QVariantList>
class MainWindow;
class DatabaseConnectDialog;
class DatabaseHistoryDialog;
class Schema2View;
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
    void setDistributionPlotManualRange(double vmin, double vmax);

    void showDistributionPlot();
    void showXYPlot();
    void showJoinHelper();
    void showDataImportDialog();
    void showDateTimeRangeWidget();
    void showQueryHistory();
    void showDatabaseHistory();

    void compareData();

    void saveStat(const QString& path);
    void saveQuery(const QString& path);

    void beforeDialog(DatabaseHistoryDialog* dialog);
    void beforeDialog(DatabaseConnectDialog *dialog);

    void afterDialog(DatabaseConnectDialog *dialog);
    void afterDialog(DatabaseHistoryDialog *dialog);

    void start();

    void next();

    void toolMysql();

    void toolMysqldump(const QStringList &tables = QStringList());

    void schemaEdit();

    void toolXJoin(const QString& conn1, const QString& conn2);

    void createTable(const QString& name, const QList<QStringList>& columns);

    void createRelation(const QString& name, const QString& childTable, const QStringList& childColumns,
                        const QString& parentTable, const QStringList& parentColumns, bool constrained);

    void createPrimaryKey(const QString& name, const QString& table, const QStringList& columns);

    void pushSchema();

    Schema2View *schemaView();
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
