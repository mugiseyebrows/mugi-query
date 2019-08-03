#ifndef DATETIME_H
#define DATETIME_H

#include <QRegularExpression>
#include <QDateTime>

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

    static QRegularExpression timeZoneRegularExpression();

    static QStringList ruMonthsShort();

    static QStringList ruMonthsLong();

    static QStringList enMonthsShort();

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
};

#endif // DATETIME_H
