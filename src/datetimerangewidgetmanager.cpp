#include "datetimerangewidgetmanager.h"

#include "datetimerangewidget.h"

DateTimeRangeWidgetManager::DateTimeRangeWidgetManager(QObject *parent) : QObject(parent)
{

}

void DateTimeRangeWidgetManager::init(DateTimeRangeWidget *widget)
{

    QDateTime dt1 = QDateTime::currentDateTime();
    QDateTime dt2 = dt1.addDays(-7);
    widget->init(dt1,dt2,{"Day","7 days","30 days","Week","Month"});

    connect(widget,&DateTimeRangeWidget::actionTriggered,[=](int index){
        QDateTime dt2;
        if (index == 3) {
            QDate date = dt1.date();
            date = date.addDays(-date.dayOfWeek() + 1);
            dt2 = QDateTime(date, QTime());
        } else if (index == 4) {
            QDate date = dt1.date();
            date = date.addDays(-date.day()+1);
            dt2 = QDateTime(date, QTime());
        } else {
            QList<int> days = {1,7,30};
            dt2 = dt1.addDays(-days[index]);
        }
        widget->setDateTime1(dt1);
        widget->setDateTime2(dt2);
    });

}
