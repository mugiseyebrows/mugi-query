#ifndef QUERYHISTORYWIDGET_H
#define QUERYHISTORYWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QSqlQuery>

namespace Ui {
class QueryHistoryWidget;
}

class QueryHistoryWidget : public QWidget
{
    Q_OBJECT

public:

    explicit QueryHistoryWidget(QWidget *parent = 0);
    ~QueryHistoryWidget();

    void refresh(const QString &connectionName);

    void updateQuery();
    QStringList selectedQueries() const;
signals:
    void appendQuery(QString,QString);

private slots:
    void on_refresh_clicked();
    void on_copy_clicked();
    void on_tableView_doubleClicked(QModelIndex index);
    void on_connectionName_currentIndexChanged(const QString &arg1);
    void on_search_clicked();
    void on_all_clicked();

private:
    Ui::QueryHistoryWidget *ui;
    QSqlQuery mQuery;
};

#endif // QUERYHISTORYWIDGET_H
