#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class SessionModel;
class SessionTab;
class History;

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

    SessionModel* model();

    int tabIndex(QTabWidget *widget, const QString &name);
    void selectTab(QTabWidget *widget, const QString &name);

    SessionTab* tab(int index);
    SessionTab* currentTab();

protected:
    History* mHistory;

public slots:
    void on_addDatabase_triggered();
    void on_sessionTree_customContextMenuRequested(QPoint pos);
    void onTreeCurrentChanged(QModelIndex, QModelIndex);
    void onSessionAdded(QString connectionName, QString name, QString namePrev);
    void onSessionRemoved(QString connectionName, QString name);
    void onAdjustSplitter();
    void onTabsCurrentChanged(int);
    void onQuery(QString query);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
