#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class SessionModel;
class SessionTab;
class History;
class QueryHistoryWidget;
class QCompleter;

#include <QSqlDatabase>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    SessionModel* model() const;
    int tabIndex(QTabWidget *widget, const QString &name);
    void selectTab(QTabWidget *widget, const QString &name);
    SessionTab* tab(int index);
    SessionTab* currentTab();

    void updateCompleter(const QString &connectionName);

    void addDatabase(bool showHistory = false);

    QString connectionName() const;

protected:
    History* mHistory;
    QueryHistoryWidget* mQueryHistory;
    QMap<QString,QCompleter*> mCompleters;
    QString mQuery;

public slots:
    void on_addDatabase_triggered();
    void on_sessionTree_customContextMenuRequested(QPoint pos);
    void onTreeCurrentChanged(QModelIndex, QModelIndex);
    void onSessionAdded(QString connectionName, QString name, QString namePrev);
    void onSessionRemoved(QString connectionName, QString name);
    void onAdjustSplitter();
    void onTabsCurrentChanged(int);
    void onQuery(QString query);
    void onShowQueryHistory();
    void onCopyQuery(QString query);
    void onAddSessionWithQuery(QString);


private slots:
    void on_saveData_triggered();

    void on_databaseHistory_triggered();

    void on_queryHistory_triggered();

    void on_addSession_triggered();

    void on_removeSession_triggered();

    void on_removeDatabase_triggered();

    void on_reconnect_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
