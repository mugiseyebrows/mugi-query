#include "datetimerangewidgetmanager.h"

#include "datetimerangewidget.h"

DateTimeRangeWidgetManager::DateTimeRangeWidgetManager(QObject *parent) : QObject(parent)
{

}

void DateTimeRangeWidgetManager::init(DateTimeRangeWidget *widget)
{


    QDateTime date2 = QDateTime::currentDateTime();
    QDateTime date1 = date2.addDays(-7);
    widget->init(date1,date2,{"Day","7 days","30 days","365 days","Week","Month","Year"});

    connect(widget,&DateTimeRangeWidget::actionTriggered,[=](int index){
        QDateTime date1;
        if (index == 4) {
            date1 = QDateTime(date2.date().addDays(-date2.date().dayOfWeek() + 1), QTime());
        } else if (index == 5) {
            date1 = QDateTime(date2.date().addDays(-date2.date().day() + 1), QTime());
        } else if (index == 6) {
            date1 = QDateTime(QDate(date2.date().year(), 1, 1), QTime());
        } else {
            QList<int> days = {1,7,30,365};
            date1 = date2.addDays(-days[index]);
        }
        widget->setDateTime1(date1);
        widget->setDateTime2(date2);
    });

}
