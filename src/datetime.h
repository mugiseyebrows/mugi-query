#ifndef DATETIME_H
#define DATETIME_H

#include <QRegularExpression>
#include <QDateTime>
#include "timezone.h"
#include "multinameenum.h"

class DateTime
{
public:

    enum Type {
        TypeUnknown,
        TypeDate,
        TypeTime,
        TypeDateTime
    };

    enum FormatDateTime {
        FormatDateTimeUndefined,
        FormatDateTimeRFC2822,   // 26 Aug 1965 21:17:22 +0300
        FormatDateTimeISO,       // 2012-05-05T05:23:06
        FormatDateTimeISOWithMs, // 1919-02-03T16:03:56.461
        // todo remove
        FormatRuLong,            // пятница, 23 сентября 2039 г. 3:48:06 MSK
        FormatRuShort,           // вт апр. 19 20:54:17 1988
        FormatEnShort            // Wed Aug  7 14:14:38 2019
    };

    enum FormatTime {
        FormatTimeUndefined,
        FormatTimeHM,
        FormatTimeHMS,
        FormatTimeHMSMS
    };

    enum FormatDate {
        FormatDateUndefined,
        FormatDateYYYYMMDD,
        FormatDateDDMMYY,
        FormatDateRuDDMonthYY,
        FormatDateEnDDMonthYY,
        FormatDateRuWeekDayMonthDDYY, // Чт окт 6 2022
        FormatDateRuWeekDayDDMonthYY  // воскресенье, 10 марта 1974 г.
    };

    static bool parse(Type type, const QString &s, QDate &date, QTime &time, QDateTime &dateTime, int minYear, bool inLocalTime, bool outUtc);
    static bool parseAsDate(const QString &s, QDate &date, int minYear);
    static bool parseAsTime(const QString &s, QTime &time);
    static bool parseAsDateTime(const QString &s, QDateTime &dateTime, int minYear, bool inLocalTime, bool outUtc);

    static void writeSamples();
    static void writeNumber();
    static void writeTimeZones();


protected:

    static bool dateTimeMaybe(const QString &s);
    static bool timeMaybe(const QString &s);
    static bool dateMaybe(const QString &s);

    static QString regExpDateTime(FormatDateTime format);
    static QString regExpDate(FormatDate format);
    static QString regExpTime(FormatTime format);
    static QString regExp(Type type, FormatDateTime formatDateTime,
                  FormatDate formatDate, FormatTime formatTime);

    static MultinameEnum ruMonths();
    static MultinameEnum enMonths();
    static MultinameEnum ruWeekDays();
    static MultinameEnum enWeekDays();

    static QString parseTimeZone(const QString &s, QDateTime &dateTime, bool *hasTimeZone);
    static QTimeZone parseTimeZone(const QString &timeZone);
    static QTime parseAmPmTime(const QString& time_, const QString& ap, const QString& format);
    static QString parseTime(const QString &s, QTime &time);
    static QString parseDate(const QString &s, QDate &date, int minYear);
    static bool parseDateTime(const QString &s, QDateTime &dateTime, int minYear, bool inLocalTime, bool outUtc);


};

#endif // DATETIME_H
