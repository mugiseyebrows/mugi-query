#ifndef DATETIME_H
#define DATETIME_H

#include <QRegularExpression>
#include <QDateTime>
#include "timezone.h"

class DateTime
{
public:
    enum Format {
        FormatRuLocaleLong, // 22 августа 2023 г. 19:35:14
        FormatRuLocaleText, // Сб дек 23 19:09:00 2006
        FormatRuLocaleShortTwoDigitYear, // 05.08.19 3:00
        Format2, // 02.08.19
        Format3, // 6 авг 19
        Format4, // 9-авг-2019
        Format5, // 07.08.19 5:00 AM
        Format6  // 07.08.19 09:08:15 PM
    };

    enum Type {
        TypeUnknown,
        TypeDate,
        TypeTime,
        TypeDateTime
    };

    enum FormatDateTime {
        FormatDateTimeUndefined,
        FormatDateTime1
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
        FormatDateRuDDMonYY,
        FormatDateRuDDMonthYY,
        FormatDateEnDDMonYY,
        FormatDateEnDDMonthYY,
    };

    static QList<FormatDate> dateFormats();

    static QList<FormatTime> timeFormats();

    static QString regExp(Type type, FormatDateTime formatDateTime,
                  FormatDate formatDate, FormatTime formatTime);

    static QString timeZoneRegExp();

    static int timeZoneOffset(const QString &timeZone, const QDateTime &dateTime);

    static QStringList ruMonthsShort();

    static QStringList ruMonthsLong();

    static QStringList enMonthsShort();

    static QStringList enMonthsLong();

    static QStringList ruWeekDaysShort();

    static QRegularExpression timeRegularExpression(Format format);

    static QRegularExpression dateRegularExpression(Format format);

    static QRegularExpression dateTimeRegularExpression(Format format);

    static QDate parseDate(const QString& s, int minYear);

    static QDateTime parseDateTime(const QString& s, int minYear, bool inLocal, bool outLocal);

    static QTime parseTime(const QString& s);

    static QList<QRegularExpression> dateRegularExpressions();

    static QList<QRegularExpression> dateTimeRegularExpressions();

    static QList<QRegularExpression> timeRegularExpressions();

    static void writeSamples();
    static void writeNumber();
    static void writeTimeZones();

    static QString parseTimeZone(const QString &s, const QDateTime &dateTime, int *offset, bool *hasTimeZone);
    static QString parseTime(const QString &s, QTime &time);
    static QString parseDate(const QString &s, QDate &date);
    static bool parse(Type type, const QString &s, QDate &date, QTime &time, QDateTime &dateTime, int minYear, bool inLocalTime, bool outLocalTime);

    static TimeZone timeZone(const QString& code);


};

#endif // DATETIME_H
