#ifndef SESSIONTAB_H
#define SESSIONTAB_H

#include <QWidget>

namespace Ui {
class SessionTab;
}

class QSqlQueryModel;
class QCompleter;
class QAbstractItemModel;
class QTableView;
class Highlighter;
class QTextDocument;
class Tokens;

class SessionTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit SessionTab(const QString& connectionName, const QString name, QWidget *parent = 0);
    ~SessionTab();

    QString connectionName() const;
    QString name() const;

    void setResult(const QStringList &queries, const QStringList errors, const QList<QSqlQueryModel*> models, const QList<int>& perf, const QList<int>& rowsAffected);

    void setTokens(const Tokens& token);

    void cleanTabs();

    void setQuery(const QString& query);

    void focusQuery();

    QSqlQueryModel *currentModel();

    void saveData();

    void copySelected(bool asList);
    QTableView *currentView();
    QTextDocument *document() const;
signals:
    void query(QString);
    void showQueryHistory();
    void addSessionWithQuery(QString);
    
protected:

    QString mConnectionName;
    QString mName;
    bool mFirstQuery;

    Ui::SessionTab *ui;
private slots:
    void on_execute_clicked();
    void on_history_clicked();
};

#endif // SESSIONTAB_H
