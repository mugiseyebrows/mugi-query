#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <QObject>
#include <QQueue>
#include <QVariantList>
class MainWindow;
class AddDatabaseDialog;
class DatabaseHistoryDialog;

#include "action.h"

class Automation : public QObject
{
    Q_OBJECT
public:

    static Automation* mInstance;
    static Automation* instance(QObject *parent = 0);

    void addDatabaseFromHistory(const QString& connectionName);
    void query(const QString &connectionName, const QString &query);
    void showSaveDataDialog();

    void beforeDialog(DatabaseHistoryDialog* dialog);
    void beforeDialog(AddDatabaseDialog *dialog);

    void afterDialog(AddDatabaseDialog *dialog);
    void afterDialog(DatabaseHistoryDialog *dialog);

    void start();

    void next();
protected:

    QQueue<Action> mQueued;
    Action mAction;

    MainWindow* mainWindow();

    Automation(QObject* parent = 0);

    AddDatabaseDialog* mAddDatabaseDialog;
    DatabaseHistoryDialog* mDatabaseHistoryDialog;

protected slots:

    void onAddDatabaseFromHistory();
    void onAddDatabaseDialog();
    void onDatabaseHistoryDialog();
    void onStart();
};

#endif // AUTOMATION_H
