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
class StatView;
class QueryModelView;

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
    QueryModelView *currentView();
    QTextDocument *document() const;

    void quoteQuery();
    void unquoteQuery();

    QueryModelView *tab(int index);

signals:
    void query(QString);
    void showQueryHistory();
    void appendQuery(QString);
    
protected:

    StatView *statView();
    QueryModelView *tab(int index, bool *insert);

    Ui::SessionTab *ui;
    QString mConnectionName;
    QString mName;
    bool mFirstQuery;
    QList<QueryModelView*> mTabs;


public slots:
    void on_execute_clicked();
    void on_history_clicked();
protected slots:
    void on_resultTabs_currentChanged(int);
};

#endif // SESSIONTAB_H
