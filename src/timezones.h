#ifndef TIMEZONES_H
#define TIMEZONES_H

#include <QString>
#include <QTimeZone>
#include "timezone.h"

class TimeZones
{
public:
    TimeZones();

    static QString parseTimeZone(const QString &s, QDateTime &dateTime, bool *hasTimeZone);
    static QTimeZone parseTimeZone(const QString &timeZoneCode);
    static QString regExp();
protected:
    static void init();
    static QMap<QString,QByteArray> mZones1;
    static QMap<QString,TimeZone> mZones2;
};

#endif // TIMEZONES_H
