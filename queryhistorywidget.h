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
    enum cols {
        col_query = 2,
    };

    explicit QueryHistoryWidget(QWidget *parent = 0);
    ~QueryHistoryWidget();

    void refresh();

signals:
    void copyQuery(QString);

private slots:
    void on_refresh_clicked();
    void on_copy_clicked();
    void on_tableView_doubleClicked(QModelIndex index);

private:
    Ui::QueryHistoryWidget *ui;
    QSqlQuery mQuery;
};

#endif // QUERYHISTORYWIDGET_H
