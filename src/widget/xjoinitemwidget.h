#ifndef XJOINITEMWIDGET_H
#define XJOINITEMWIDGET_H

#include <QWidget>
class QSqlQueryModel;

namespace Ui {
class XJoinItemWidget;
}

class XJoinItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit XJoinItemWidget(QWidget *parent = nullptr);
    ~XJoinItemWidget();

    void init(const QStringList& connectionNames);

    QString connectionName() const;

    QString query() const;

    QSqlQueryModel* model();

    QStringList columns() const;

signals:
    void columnsChanged();
    void queryExecuted();

private slots:
    void on_execute_clicked();

private:
    Ui::XJoinItemWidget *ui;
};

#endif // XJOINITEMWIDGET_H
