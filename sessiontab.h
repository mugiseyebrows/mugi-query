#ifndef SESSIONTAB_H
#define SESSIONTAB_H

#include <QWidget>

namespace Ui {
class SessionTab;
}

class QSqlQueryModel;
class QCompleter;
class QAbstractItemModel;

class SessionTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit SessionTab(const QString& connectionName, const QString name, QWidget *parent = 0);
    ~SessionTab();

    QString connectionName() const;
    QString name() const;

    void setResult(const QStringList &queries, const QStringList errors, const QList<QSqlQueryModel*> models, const QList<int>& perf, const QList<int>& rowsAffected);

    void setCompleter(QCompleter* completer);

    void cleanTabs();

    void setQuery(const QString& query);

    QSqlQueryModel *currentModel();

protected:



public slots:
    void on_execute_clicked();

signals:
    void query(QString);
    void showQueryHistory();
    
protected:

    QString mConnectionName;
    QString mName;

    Ui::SessionTab *ui;
private slots:
    void on_history_clicked();
    void on_save_clicked();
};

#endif // SESSIONTAB_H
