#ifndef DATETIMERANGEWIDGET_H
#define DATETIMERANGEWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QDateTime>

namespace Ui {
class DateTimeRangeWidget;
}

class DateTimeRangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeRangeWidget(QWidget *parent = 0);
    ~DateTimeRangeWidget();
    void init(const QDateTime &dateTime1, const QDateTime &dateTime2, const QStringList &options);
    QDateTime dateTime1() const;
    QDateTime dateTime2() const;
    void setDateTime1(const QDateTime& value);
    void setDateTime2(const QDateTime& value);

protected:
    Ui::DateTimeRangeWidget *ui;
    QMenu *mMenu1;

signals:
    void actionTriggered(int);
    void dateTime1Changed(QDateTime);
    void dateTime2Changed(QDateTime);
};

#endif // DATETIMERANGEWIDGET_H
