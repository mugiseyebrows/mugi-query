#include "datetimerangewidgetmanager.h"

#include "datetimerangewidget.h"


DateTimeRangeWidgetManager::DateTimeRangeWidgetManager(QObject *parent) : QObject(parent)
{

}

void DateTimeRangeWidgetManager::setDay() { onActionTriggered(0); }
void DateTimeRangeWidgetManager::set7days() { onActionTriggered(1); }
void DateTimeRangeWidgetManager::set30days() { onActionTriggered(2); }
void DateTimeRangeWidgetManager::set365days() { onActionTriggered(3); }
void DateTimeRangeWidgetManager::setWeek() { onActionTriggered(4); }
void DateTimeRangeWidgetManager::setMonth() { onActionTriggered(5); }
void DateTimeRangeWidgetManager::setYear() { onActionTriggered(6); }

void DateTimeRangeWidgetManager::init(DateTimeRangeWidget *widget)
{
    mWidget = widget;
    mDate2 = QDateTime::currentDateTime();
    QDateTime date1 = mDate2.addDays(-7);
    mWidget->init(date1,mDate2,{"Day","7 days","30 days","365 days","Week","Month","Year"});
    connect(mWidget,SIGNAL(actionTriggered(int)),this,SLOT(onActionTriggered(int)));
}

void DateTimeRangeWidgetManager::onActionTriggered(int index)
{
    QDateTime date1;
    if (index == 4) {
        date1 = QDateTime(mDate2.date().addDays(-mDate2.date().dayOfWeek() + 1), QTime());
    } else if (index == 5) {
        date1 = QDateTime(mDate2.date().addDays(-mDate2.date().day() + 1), QTime());
    } else if (index == 6) {
        date1 = QDateTime(QDate(mDate2.date().year(), 1, 1), QTime());
    } else {
        QList<int> days = {1,7,30,365};
        date1 = mDate2.addDays(-days[index]);
    }
    mWidget->setDateTime1(date1);
    mWidget->setDateTime2(mDate2);
}
