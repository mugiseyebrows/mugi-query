#ifndef DATETIMERANGEWIDGETMANAGER_H
#define DATETIMERANGEWIDGETMANAGER_H

#include <QObject>
#include <QDateTime>
class DateTimeRangeWidget;

class DateTimeRangeWidgetManager : public QObject
{
    Q_OBJECT
public:
    explicit DateTimeRangeWidgetManager(QObject *parent = nullptr);

    void init(DateTimeRangeWidget* widget);

    void setDay();
    void set7days();
    void set30days();
    void set365days();
    void setWeek();
    void setMonth();
    void setYear();
protected:
    DateTimeRangeWidget* mWidget;
    QDateTime mDate2;
signals:

public slots:

    void onActionTriggered(int index);
};

#endif // DATETIMERANGEWIDGETMANAGER_H
