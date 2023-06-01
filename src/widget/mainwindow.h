#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class SessionModel;
class SessionTab;
class History;
class QueryHistoryWidget;
class QCompleter;
class Highlighter;
class JoinHelperWidgets;
class SchemaModel;
class DataImportWidgets;
class RelationsModel;
class QSqlQueryModel;

#include "tokens.h"

#include <QSqlDatabase>
#include <enums.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    SessionModel* model() const;
    int tabIndex(QTabWidget *widget, const QString &name);
    void selectTab(QTabWidget *widget, const QString &name);
    SessionTab* tab(int index);
    SessionTab* currentTab();

    void updateTokens(const QString &connectionName);
    void pushTokens(const QString &connectionName);

    void databaseConnect(bool showHistory = false);

    QString connectionName() const;
    QSqlDatabase database() const;

    void selectTab(const QString &name);
    QMenu* selectionMenu() const;

    SchemaModel *schemaModel();
protected:
    void closeEvent(QCloseEvent *event);
    QueryHistoryWidget* mQueryHistory;
    QMap<QString,Tokens> mTokens;
    JoinHelperWidgets* mJoinHelpers;
    DataImportWidgets* mDataImport;

    QString mQuery;

    QList<QSqlQueryModel*> mCompareModels;

    void copySelected(CopyFormat fmt);

    int lastTabIndex(const QString &connectionName);
    void selectDatabase(const QString &connectionName);
    void updateSchemaModel();
    QStringList schemaTreeSelectedTables();

protected slots:

    void on_databaseConnect_triggered();
    void on_databaseDisconnect_triggered();
    void on_databaseReconnect_triggered();
    void on_databaseHistory_triggered();

    void on_sessionAdd_triggered();
    void on_sessionRemove_triggered();

    void on_queryQuote_triggered();
    void on_queryUnquote_triggered();
    void on_queryHistory_triggered();
    void on_queryHelp_triggered();
    void on_queryJoin_triggered();
    void on_queryExecute_triggered();



    void on_selectionCopy_triggered();
    void on_selectionCopyAsList_triggered();
    void on_selectionCopyAsCondition_triggered();

    void on_settingsFormat_triggered();

    void on_sessionTree_customContextMenuRequested(QPoint pos);
    void onTreeCurrentChanged(QModelIndex, QModelIndex);
    void onSessionAdded(QString connectionName, QString name, QString namePrev);
    void onSessionRemoved(QString connectionName, QString name);
    void onAdjustSplitter();
    void onTabsCurrentChanged(int);
    void onQuery(QString query);
    void onShowQueryHistory();
    //void onAddSessionWithQuery(QString);
    void onAppendQuery(const QString &connectionName, QString);
    void on_schemaTree_customContextMenuRequested(const QPoint &pos);

    void on_schemaTree_doubleClicked(const QModelIndex &index);

    void onShowDatabaseDialog(bool showHistory);

    void on_dataSave_triggered();
    void on_dataImport_triggered();
    void on_dataCompare_triggered();

    void on_toolsMysql_triggered();
    void on_toolsMysqldump_triggered();

    void on_codePython_triggered();
    void on_codePandas_triggered();

    void on_schemaEdit_triggered();

    void on_toolsJoin_triggered();

    friend class Automation;

    void on_codeCopyOrder_triggered();

private slots:
    void on_dataCompareTable_triggered();

    void on_dataCompareDatabase_triggered();

    void on_dataStatistics_triggered();

    void on_codeRelations_triggered();

    void on_codePrimaryKeys_triggered();

    void on_dataTruncate_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
