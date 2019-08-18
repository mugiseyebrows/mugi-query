#ifndef DATETIMERANGEWIDGETMANAGER_H
#define DATETIMERANGEWIDGETMANAGER_H

#include <QObject>
class DateTimeRangeWidget;

class DateTimeRangeWidgetManager : public QObject
{
    Q_OBJECT
public:
    explicit DateTimeRangeWidgetManager(QObject *parent = nullptr);

    void init(DateTimeRangeWidget* widget);

signals:

public slots:
};

#endif // DATETIMERANGEWIDGETMANAGER_H
