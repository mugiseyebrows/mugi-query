#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class SessionModel;
class SessionTab;

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

public slots:
    void onTreeCurrentChanged(QModelIndex, QModelIndex);
    void onSessionAdded(QString db, QString name, QString namePrev);
    void onSessionRemoved(QString name);
    void onAdjustSplitter();
    void onTabsCurrentChanged(int);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
