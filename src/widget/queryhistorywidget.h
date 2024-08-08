#ifndef QUERYHISTORYWIDGET_H
#define QUERYHISTORYWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QSqlQuery>

namespace Ui {
class QueryHistoryWidget;
}

class RichHeaderView;
class DateTimeRangeWidget;
class QComboBox;
class QLineEdit;
class DateRangeWidget;

class QueryHistoryWidget : public QWidget
{
    Q_OBJECT

public:

    explicit QueryHistoryWidget(QWidget *parent = 0);
    ~QueryHistoryWidget();

    void refresh(const QString &connectionName);


    QStringList selectedQueries() const;
    RichHeaderView *headerView();
    DateRangeWidget *dateEdit();
    QComboBox *connectionNameEdit();
    QLineEdit *queryEdit();
    QWidget *edit(int column);
signals:
    void appendQuery(QString,QString);

private slots:
    void on_refresh_clicked();
    void on_copy_clicked();
    void on_tableView_doubleClicked(QModelIndex index);
    void onUpdateQuery();

private:
    Ui::QueryHistoryWidget *ui;
    QSqlQuery mQuery;
};

#endif // QUERYHISTORYWIDGET_H
