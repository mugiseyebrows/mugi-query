#include "datetime.h"
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QTimeZone>
#include "timezones.h"

namespace {

int twoDigitYear(int year, int minYear) {
    int year_ = (minYear / 100) * 100 + year;
    if (year_ < minYear) {
        year_ = (minYear / 100 + 1) * 100 + year;
    }
    return year_;
}

QString range(int a, int b) {
    return a == b ? QString("%1").arg(a) : QString("[%1-%2]").arg(a).arg(b);
}

QString number(int min, int max, bool cap = true) {
    Q_ASSERT(min < 100 && max < 100 && max >= min);
    QStringList result;
    int min1 = min / 10;
    int max1 = max / 10;
    int min2 = min % 10;
    int max2 = max % 10;

    bool up = min2 == 0;
    bool down = max2 == 9;

    if (min1 == max1) {
        result << range(min1,min1) + range(min2,max2);
        if (min1 == 0) {
            result << range(min2,max2);
        }
    } else {
        if (!up) {
            result << range(min1,min1) + range(min2,9);   
        }
        if ((max1 - min1) > 1) {
            result << range(up ? min1 : min1 + 1, down ? max1 : max1 - 1) + range(0,9);
        }
        if (!down) {
            result << range(max1,max1) + range(0,max2);
        }
        if (min1 == 0) {
            result << range(min2,9);
        }
    }
    return (cap ? "(" : "(?:") + result.join("|") + ")";
}


QString head(const QString& s) {
    return "^" + s;
}

QString whole(const QString& s) {
    return "^" + s + "$";
}

QString group(const QString& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s + ")";
}

} // namespace

QString DateTime::regExpDateTime(FormatDateTime format) {
    return regExp(TypeDateTime, format, FormatDateUndefined, FormatTimeUndefined);
}

QString DateTime::regExpDate(FormatDate format) {
    return regExp(TypeDate, FormatDateTimeUndefined, format, FormatTimeUndefined);
}

QString DateTime::regExpTime(FormatTime format) {
    return regExp(TypeTime, FormatDateTimeUndefined, FormatDateUndefined, format);
}

QString DateTime::regExp(DateTime::Type type, FormatDateTime formatDateTime, DateTime::FormatDate formatDate, DateTime::FormatTime formatTime)
{
    QString notDigit = "[^0-9]";
    QString yearDot = group("\\sг.",false) + "?";

    QString hm = number(0,23,false) + ":" + number(0,59,false);
    QString hms = hm + ":" + number(0,59,false);
    QString hmsms = hms + "[.][0-9]{1,3}";

    QString fourDigits = "[0-9]{4}";

    static MultinameEnum ruMonths = DateTime::ruMonths();
    static MultinameEnum enMonths = DateTime::enMonths();
    static MultinameEnum ruWeekDays = DateTime::ruWeekDays();
    static MultinameEnum enWeekDays = DateTime::enWeekDays();

    if (type == TypeDateTime){
        static QMap<FormatDateTime, QString> exps = {
            // 26 Aug 1965 21:17:22 +0300
            {FormatDateTimeRFC2822, number(1,31) + "\\s" + enMonths.regExp() + "\\s" + fourDigits + "\\s" + hms + "\\s[+-][0-9]{4}"},
            // 2012-05-05T05:23:06
            {FormatDateTimeISO, fourDigits + "-" + number(1,12) + "-" + number(1,31) + "T" + hms},
            // 1919-02-03T16:03:56.461
            {FormatDateTimeISOWithMs, "[0-9]{4}-" + number(1,12) + "-" + number(1,31) + "T" + hmsms},
            // пятница, 23 сентября 2039 г. 3:48:06 MSK
            {FormatRuLong, ruWeekDays.regExp() + "[,.]?\\s" + number(1,31) + "\\s" + ruMonths.regExp() + "[.]?\\s" + group(fourDigits) + yearDot + "\\s" + group(hms) + "\\s" + TimeZones::regExp()},
            // вт апр. 19 20:54:17 1988
            {FormatRuShort, ruWeekDays.regExp() + "[,.]?\\s" + ruMonths.regExp() + "[.]?\\s{1,2}" + number(1,31) + "\\s" + group(hms) + "\\s" + group(fourDigits)},
            // Wed Aug  7 14:14:38 2019
            {FormatEnShort, enWeekDays.regExp() + "[,.]?\\s" + enMonths.regExp() + "[.]?\\s{1,2}" + number(1,31) + "\\s" + group(hms) + "\\s" + group(fourDigits)},
        };
        return exps[formatDateTime];
    } else if (type == TypeDate) {
        static QMap<FormatDate, QString> exps = {
            {FormatDateYYYYMMDD, group("[0-9]{4}") + notDigit + number(1,12) + notDigit + number(1,31)},
            {FormatDateDDMMYY, number(1,31) + notDigit + number(1,12) + notDigit + group("[0-9]{2,4}") + yearDot },
            {FormatDateRuDDMonthYY, number(1,31) + notDigit + ruMonths.regExp() + "[.,]?" + notDigit + group("[0-9]{2,4}") + yearDot },
            {FormatDateEnDDMonthYY, number(1,31) + notDigit + enMonths.regExp() + notDigit + group("[0-9]{2,4}")},
            // Чт окт 6 2022
            {FormatDateRuWeekDayMonthDDYY, ruWeekDays.regExp() + "[.,]?\\s" + ruMonths.regExp() + "[.,]?\\s" + number(0,31) + "[.,]?\\s" + group("[0-9]{2,4}") + yearDot},
            // воскресенье, 10 марта 1974 г.
            {FormatDateRuWeekDayDDMonthYY, ruWeekDays.regExp() + "[.,]?\\s" + number(0,31) + "[.,]?\\s" + ruMonths.regExp() + "[.,]?\\s" + group("[0-9]{2,4}") + yearDot}
        };
        return exps[formatDate];
    } else if (type == TypeTime) {
        static QMap<FormatTime, QString> exps = {
            {FormatTimeHM, group(hm) + "(\\sAM|\\sPM)?" },
            {FormatTimeHMS, group(hms) + "(\\sAM|\\sPM)?"},
            {FormatTimeHMSMS, group(hmsms) + "(\\sAM|\\sPM)?"},
        };
        return exps[formatTime];
    }
    return QString();
}

bool DateTime::dateTimeMaybe(const QString& s) {
    QRegularExpression fourDigitGroups("[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+");
    return fourDigitGroups.match(s).hasMatch() && s.indexOf(":") > 0;
}

bool DateTime::timeMaybe(const QString& s) {
    return s.indexOf(":") > 0;
}

bool DateTime::dateMaybe(const QString& s) {
    QRegularExpression twoDigitGroups("[0-9]+[^0-9]+[0-9]+");
    return twoDigitGroups.match(s).hasMatch();
}

QTime DateTime::parseAmPmTime(const QString& time_, const QString& ap, const QString& format) {
    QTime time = QTime::fromString(time_,format);
    // AM/PM parsing doesn't work on some locales
    if (!time.isValid()) {
        return QTime();
    }
    QString ap_ = ap.trimmed().toLower();

    if (ap_ == "pm") {
        if (!(time.hour() == 12 && time.minute() == 0 && time.second() == 0 && time.msec() == 0)) { // 12:00 PM is noon 12:00 AM is midnight
            time = time.addSecs(12 * 3600);
        }
    } else if (ap_ == "am") {
        if (time.hour() == 12 && time.minute() == 0 && time.second() == 0 && time.msec() == 0) {
            time = time.addSecs(-12 * 3600);
        }
    } else {
        qDebug() << "AM / PM error";
        return QTime();
    }
    return time;
}

QString DateTime::parseTime(const QString& s, QTime& time) {
    QList<FormatTime> formats = {FormatTimeHMSMS,
                                 FormatTimeHMS,
                                 FormatTimeHM};
    QStringList formats_ = {"h:m:s.z","h:m:s","h:m"};
    QList<QRegularExpression> exps;
    foreach (FormatTime format, formats) {
        exps << QRegularExpression(head(regExpTime(format) + "\\s?"), QRegularExpression::CaseInsensitiveOption);
    }
    for(int i=0;i<formats.size();i++) {
        QRegularExpression rx = exps[i];
        QRegularExpressionMatch m = rx.match(s);
        if (m.hasMatch()) {
            if (m.lastCapturedIndex() == 2) {
                time = parseAmPmTime(m.captured(1),m.captured(2),formats_[i]);
            } else {
                time = QTime::fromString(m.captured(1),formats_[i]);
            }

            QString tail = s.mid(m.capturedLength());
            return tail;
        }
    }
    return s;
}

bool DateTime::parseDateTime(const QString& s, QDateTime& dateTime,
                             int minYear, bool inLocalTime, bool outUtc) {

    QList<FormatDateTime> formats = {FormatDateTimeRFC2822, FormatDateTimeISO, FormatDateTimeISOWithMs};
    QList<Qt::DateFormat> formats_ = {Qt::RFC2822Date, Qt::ISODate, Qt::ISODateWithMs};
    QList<QRegularExpression> exps;
    foreach(FormatDateTime format, formats) {
        exps << QRegularExpression(whole(regExpDateTime(format)),QRegularExpression::CaseInsensitiveOption);
    }

    for(int i=0;i<formats.size();i++) {
        Qt::DateFormat format = formats_[i];
        QRegularExpressionMatch m = exps[i].match(s);
        if (m.hasMatch()) {
            QDateTime dateTime_ = QDateTime::fromString(s,format);
            if (!dateTime_.isValid()) {
                qDebug() << __FILE__ << __LINE__ << s << format;
                return false;
            }
            if (format != Qt::RFC2822Date) {
                dateTime_ = QDateTime(dateTime_.date(), dateTime_.time(), inLocalTime ? Qt::LocalTime : Qt::UTC);
            }
            dateTime = outUtc ? dateTime_.toUTC() : dateTime_;
            return true;
        }
    }

    static MultinameEnum ruMonths = DateTime::ruMonths();
    static MultinameEnum enMonths = DateTime::enMonths();

    QRegularExpression rx;
    QRegularExpressionMatch m;
    QString exp = whole(regExpDateTime(FormatRuLong));
    rx = QRegularExpression(exp,QRegularExpression::CaseInsensitiveOption);
    m = rx.match(s);
    if (m.hasMatch()) {
        // 1          2    3          4         5         6
        // (пятница), (23) (сентября) (2039) г. (3:48:06) (MSK)
        int day = m.captured(2).toInt();
        int month = ruMonths.indexOf(m.captured(3).toLower()) + 1;
        int year = m.captured(4).toInt();
        QTime time_ = QTime::fromString(m.captured(5),"h:m:s");
        if (!time_.isValid()) {
            qDebug() << __FILE__ << __LINE__ << m.captured(5);
            return false;
        }
        QTimeZone timeZone = TimeZones::parseTimeZone(m.captured(6));
        if (!timeZone.isValid()) {
            qDebug() << __FILE__ << __LINE__ << m.captured(6);
            return false;
        }
        QDateTime dateTime_(QDate(year,month,day),time_,timeZone);
        dateTime = outUtc ? dateTime_.toUTC() : dateTime_;
        return true;
    }

    exp = whole(regExpDateTime(FormatRuShort));
    rx = QRegularExpression(exp,QRegularExpression::CaseInsensitiveOption);
    m = rx.match(s);

    if (m.hasMatch()) {
        // 1    2      3    4          5
        // (вт) (апр). (19) (20:54:17) (1988)

        int day = m.captured(3).toInt();
        int month = ruMonths.indexOf(m.captured(2).toLower()) + 1;
        int year = m.captured(5).toInt();

        if (month < 1) {
            qDebug() << __FILE__ << __LINE__ << m.captured(2).toLower();
        }

        QTime time_ = QTime::fromString(m.captured(4),"h:m:s");
        if (!time_.isValid()) {
            qDebug() << __FILE__ << __LINE__ << m.captured(4);
            return false;
        }
        QDateTime dateTime_(QDate(year,month,day),time_,inLocalTime ? Qt::LocalTime : Qt::UTC);
        dateTime = outUtc ? dateTime_.toUTC() : dateTime_;
        return true;
    }

    exp = whole(regExpDateTime(FormatEnShort));
    rx = QRegularExpression(exp,QRegularExpression::CaseInsensitiveOption);
    m = rx.match(s);

    if (m.hasMatch()) {
        // 1     2      3   4          5
        // (Wed) (Aug)  (7) (14:14:38) (2019)

        int day = m.captured(3).toInt();
        int month = enMonths.indexOf(m.captured(2).toLower()) + 1;
        int year = m.captured(5).toInt();

        if (month < 1) {
            qDebug() << __FILE__ << __LINE__ << m.captured(2).toLower();
        }

        QTime time_ = QTime::fromString(m.captured(4),"h:m:s");
        if (!time_.isValid()) {
            qDebug() << __FILE__ << __LINE__ << m.captured(4);
            return false;
        }
        QDateTime dateTime_(QDate(year,month,day),time_,inLocalTime ? Qt::LocalTime : Qt::UTC);
        dateTime = outUtc ? dateTime_.toUTC() : dateTime_;
        return true;
    }

    QDate date_;
    QTime time_;
    QString s_ = parseDate(s, date_, minYear);
    if (!date_.isValid()) {
        return false;
    }
    s_ = parseTime(s_, time_);
    if (!time_.isValid()) {
        return false;
    }
    bool hasTimeZone;
    QDateTime dateTime_(date_,time_);
    s_ = TimeZones::parseTimeZone(s_, dateTime_, &hasTimeZone);
    if (!s_.isEmpty()) {
        return false;
    }
    if (!dateTime_.isValid()) {
        qDebug() << __FILE__ << __LINE__;
        return false;
    }
    if (!hasTimeZone) {
        dateTime_ = QDateTime(date_, time_, inLocalTime ? Qt::LocalTime : Qt::UTC);
    }
    dateTime = outUtc ? dateTime_.toUTC() : dateTime_;
    return true;
}

QString DateTime::parseDate(const QString& s, QDate& date, int minYear) {

    QList<FormatDate> formats = {FormatDateYYYYMMDD,
                                 FormatDateDDMMYY,
                                 FormatDateRuDDMonthYY,
                                 FormatDateEnDDMonthYY,
                                 FormatDateRuWeekDayMonthDDYY,
                                 FormatDateRuWeekDayDDMonthYY};

    QList<QRegularExpression> exps;
    foreach (FormatDate format, formats) {
        exps << QRegularExpression(head(regExpDate(format) + "\\s?"), QRegularExpression::CaseInsensitiveOption);
    }

    static MultinameEnum ruMonths = DateTime::ruMonths();
    static MultinameEnum enMonths = DateTime::enMonths();

    for(int i=0;i<formats.size();i++) {
        FormatDate format = formats[i];
        QRegularExpression rx = exps[i];
        QRegularExpressionMatch m = rx.match(s);

        if (m.hasMatch()) {
            QString year_;
            if (format == FormatDateYYYYMMDD) {
                year_ = m.captured(1);
            } else if (format == FormatDateRuWeekDayMonthDDYY || format == FormatDateRuWeekDayDDMonthYY) {
                year_ = m.captured(4);
            } else {
                year_ = m.captured(3);
            }
            int year = year_.toInt();
            if (year_.size() < 3) {
                year = twoDigitYear(year, minYear);
            }
            int month = -1;
            if (format == FormatDateYYYYMMDD || format == FormatDateDDMMYY) {
                month = m.captured(2).toInt();
            } else if (format == FormatDateRuDDMonthYY) {
                month = ruMonths.indexOf(m.captured(2).toLower()) + 1;
            } else if (format == FormatDateEnDDMonthYY) {
                month = enMonths.indexOf(m.captured(2).toLower()) + 1;
            } else if (format == FormatDateRuWeekDayMonthDDYY) {
                month = ruMonths.indexOf(m.captured(2).toLower()) + 1;
            } else if (format == FormatDateRuWeekDayDDMonthYY) {
                month = ruMonths.indexOf(m.captured(3).toLower()) + 1;
            }
            if (month < 1) {
                qDebug() << __FILE__ << __LINE__ << m.captured(2).toLower();
            }

            int day;
            if (format == FormatDateYYYYMMDD || format == FormatDateRuWeekDayMonthDDYY) {
                day = m.captured(3).toInt();
            } else if (format == FormatDateRuWeekDayDDMonthYY) {
                day = m.captured(2).toInt();
            } else {
                day = m.captured(1).toInt();
            }

            date = QDate(year,month,day);
            return s.mid(m.capturedLength());
        }
    }
    return s;
}

bool DateTime::parse(Type type, const QString& s, QDate& date, QTime& time,
                     QDateTime& dateTime, int minYear, bool inLocalTime, bool outUtc) {

    if (type == TypeUnknown) {
        QDate date_;
        QTime time_;
        QDateTime dateTime_;
        if (parse(TypeDateTime, s, date_, time_, dateTime_, minYear, inLocalTime, outUtc)) {
            dateTime = dateTime_;
            return true;
        }
        if (parse(TypeTime, s, date_, time_, dateTime_, minYear, inLocalTime, outUtc)) {
            time = time_;
            return true;
        }
        if (parse(TypeDate, s, date_, time_, dateTime_, minYear, inLocalTime, outUtc)) {
            date = date_;
            return true;
        }
        return false;

    } else if (type == TypeDate) {

        if (!dateMaybe(s)) {
            return false;
        }

        QDate date_;
        QString s_ = parseDate(s, date_, minYear);
        if (!s_.isEmpty() || !date_.isValid()) {
            return false;
        }
        date = date_;
        return true;

    } else if (type == TypeTime) {
        if (!timeMaybe(s)) {
            return false;
        }
        QTime time_;
        QString s_ = parseTime(s, time_);
        if (!s_.isEmpty() || !time_.isValid()) {
            return false;
        }
        time = time_;
        return true;

    } else if (type == TypeDateTime) {

        if (!dateTimeMaybe(s)) {
            return false;
        }

        return parseDateTime(s, dateTime, minYear, inLocalTime, outUtc);
    }

    return false;
}

bool DateTime::parseAsDate(const QString &s, QDate &date, int minYear)
{
    QTime time;
    QDateTime dateTime;
    return parse(TypeDate,s,date,time,dateTime,minYear,true,true);
}

bool DateTime::parseAsTime(const QString &s, QTime &time)
{
    QDate date;
    QDateTime dateTime;
    return parse(TypeTime,s,date,time,dateTime,1950,true,true);
}

bool DateTime::parseAsDateTime(const QString &s, QDateTime &dateTime, int minYear, bool inLocalTime, bool outUtc)
{
    QDate date;
    QTime time;
    return parse(TypeDateTime,s,date,time,dateTime,minYear,inLocalTime,outUtc);
}

MultinameEnum DateTime::ruMonths() {
    return MultinameEnum(
    {"янв", "фев", "мар", "апр", "май", "июн", "июл", "авг", "сен", "окт", "ноя", "дек"},
    {"янв","февр","мар","апр","мая","июн","июл","авг","сент","окт","нояб","дек"},
    {"января","февраля","марта","апреля","мая","июня","июля","августа","сентября","октября","ноября","декабря"});
}

MultinameEnum DateTime::enMonths() {
    return MultinameEnum(
    {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"},
    {"january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"});
}

MultinameEnum DateTime::ruWeekDays() {
    return MultinameEnum(
    {"пн", "вт", "ср", "чт", "пт", "сб", "вс"},
    {"понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье"});
}

MultinameEnum DateTime::enWeekDays() {
    return MultinameEnum(
    {"mon","tue","wed","thu","fri","sat","sun"},
    {"monday","tuesday","wednesday","thursday","friday","saturday","sunday"});
}

void DateTime::writeSamples()
{
    static QList<Qt::DateFormat> dateFormats = {Qt::TextDate,
                                            Qt::ISODate,
#if 0
                                            Qt::SystemLocaleShortDate,
                                            Qt::SystemLocaleLongDate,
                                            Qt::DefaultLocaleShortDate,
                                            Qt::DefaultLocaleLongDate,
#endif
                                            Qt::RFC2822Date,
                                            Qt::ISODateWithMs};

    QStringList dateFormatNames = {"TextDate",
                                   "ISODate",
#if 0
                                   "SystemLocaleShortDate",
                                   "SystemLocaleLongDate",
                                   "DefaultLocaleShortDate",
                                   "DefaultLocaleLongDate",
#endif
                                   "RFC2822Date",
                                   "ISODateWithMs"};

    QDir dir(".");

    dir.mkpath("data");
    dir.cd("data");

    for(int i=0;i<dateFormats.size();i++) {
        Qt::DateFormat format = dateFormats[i];
        QString formatName = dateFormatNames[i];

        QStringList subdirNames = {"date","dateTime","time"};

        for (int sw=0; sw < 3; sw++) {
            QString sw_ = subdirNames[sw];
            dir.mkpath(sw_);
            QDir subdir(dir.filePath(sw_));
            QString filePath = subdir.filePath(formatName + ".txt");

            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug() << "cannot open" << filePath;
                continue;
            }

            QTextStream stream(&file);
            stream.setEncoding(QStringConverter::Utf8);

            for(int j=0;j<100;j++) {

                int y = 1900 + (rand() % 200);
                int M = 1 + (rand() % 12);
                int d = 1 + (rand() % 28);

                //M = 1 + (j % 12);
                //d = 1 + (j % 28);
                //y = 2000;
                //M = 1;

                int h = rand() % 24;
                int m = rand() % 60;
                int s = rand() % 60;
                int ms = rand() % 1000;

                QDate date(y,M,d);
                QTime time(h,m,s,ms);
                QDateTime dateTime(date,time);
                //stream << dateTime.toString(format) << "\n";

                if (sw == 0) {

                    if (1) {
                        stream << QString("Test%4Sample(Q%4::fromString(\"%1\",%5),\"%2\",Qt::%3),\n")
                                  .arg(dateTime.date().toString(Qt::ISODate))
                                  .arg(dateTime.date().toString(format))
                                  .arg(formatName)
                                  .arg("Date")
                                  .arg("Qt::ISODate");
                    } else {
                        stream << dateTime.date().toString("dd-MMMM-yyyy") << "\n";
                    }

                } else if (sw == 1) {

                    if (1) {

                        stream << QString("Test%4Sample(Q%4::fromString(\"%1\",%5),\"%2\",Qt::%3),\n")
                                  .arg(dateTime.toString(format == Qt::RFC2822Date ? Qt::RFC2822Date : Qt::ISODateWithMs))
                                  .arg(dateTime.toString(format))
                                  .arg(formatName)
                                  .arg("DateTime")
                                  .arg(format == Qt::RFC2822Date ? "Qt::RFC2822Date" : "Qt::ISODateWithMs");
                    } else {
                        stream << dateTime.toString(format) << "\n";
                    }

                } else if (sw == 2) {
                    stream << QString("Test%4Sample(Q%4::fromString(\"%1\",%5),\"%2\",Qt::%3),\n")
                              .arg(dateTime.toString(Qt::ISODateWithMs))
                              .arg(dateTime.time().toString(format))
                              .arg(formatName)
                              .arg("Time")
                              .arg("Qt::ISODateWithMs");
                }
            }
        }
    }

    qDebug() << "samples writen";

}

void DateTime::writeTimeZones() {

    QList<QByteArray> timeZoneIds = QTimeZone::availableTimeZoneIds();

    QDir dir(".");
    dir.mkpath("data");
    dir.cd("data");

    QString filePath = dir.filePath("zones.txt");

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "cannot open" << filePath;
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    foreach(const QByteArray& timeZoneId, timeZoneIds) {
        QDateTime dateTime(QDate(2000,1,1),QTime(10,11,12),QTimeZone(timeZoneId));
        QString abbr = dateTime.timeZoneAbbreviation();
        stream << abbr
               << "\t" << QString::fromUtf8(timeZoneId)
               << "\n";
    }

    qDebug() << filePath << "writen";
}


void DateTime::writeNumber() {
    QDir dir(".");

    dir.mkpath("data");
    dir.cd("data");

    QString filePath = dir.filePath("number.txt");

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "cannot open" << filePath;
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    for (int i=0;i<500;i++) {
        int a = rand() % 100;
        int b = rand() % 100;
        if (b > a) {
            stream << a << " " << b << " " << number(a,b) << "\n";
        }
    }

    qDebug() << filePath << "writen";
}
