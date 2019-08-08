#include "datetime.h"
#include <QDir>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>
#include <QTimeZone>

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

QString tail(const QString& s) {
    return s + "$";
}

QString whole(const QString& s) {
    return "^" + s + "$";
}

QString group(const QString& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s + ")";
}

QString group(const QStringList& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s.join("|") + ")";
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
            {FormatRuLong, ruWeekDays.regExp() + "[,.]?\\s" + number(1,31) + "\\s" + ruMonths.regExp() + "[.]?\\s" + group(fourDigits) + yearDot + "\\s" + group(hms) + timeZoneRegExp()},
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
            {FormatDateEnDDMonthYY, number(1,31) + notDigit + enMonths.regExp() + notDigit + group("[0-9]{2,4}")}
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
        QRegularExpressionMatch m = exps[i].match(s);
        if (m.hasMatch()) {
            dateTime = QDateTime::fromString(s,formats_[i]);
            if (dateTime.isValid()) {
                return true;
            }
            return false;
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
        QTimeZone timeZone = parseTimeZone(m.captured(6));
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
    s_ = parseTimeZone(s_, dateTime_, &hasTimeZone);
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
                                 FormatDateEnDDMonthYY};

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
            QString year_ = format == FormatDateYYYYMMDD ? m.captured(1) : m.captured(3);
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
            }
            if (month < 1) {
                qDebug() << __FILE__ << __LINE__ << m.captured(2).toLower();
            }
            int day = format == FormatDateYYYYMMDD ? m.captured(3).toInt() : m.captured(1).toInt();
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

// TODO: store in json
TimeZone DateTime::timeZone(const QString &code)
{
    // see timezones.js
    static QMap<QString,TimeZone> zones = {
        {"A", TimeZone("A", "Alpha Time Zone", "UTC+01:00", "UTC +1", 3600)},
        {"ACDT", TimeZone("ACDT", "Australian Central Daylight Time", "", "UTC +10:30", 37800)},
        {"ACST", TimeZone("ACST", "Australian Central Standard Time", "UTC+09:30", "UTC +9:30", 34200)},
        {"ACWST", TimeZone("ACWST", "Australian Central Western Standard Time", "Australia/Eucla", "UTC +8:45", 31500)},
        {"AEDT", TimeZone("AEDT", "Australian Eastern Daylight Time", "UTC+11:00", "UTC +11", 39600)},
        {"AEST", TimeZone("AEST", "Australian Eastern Standard Time", "UTC+10:00", "UTC +10", 36000)},
        {"AFT", TimeZone("AFT", "Afghanistan Time", "Asia/Kabul", "UTC +4:30", 16200)},
        {"AKDT", TimeZone("AKDT", "Alaska Daylight Time", "UTC-08:00", "UTC -8", -28800)},
        {"AKST", TimeZone("AKST", "Alaska Standard Time", "UTC-09:00", "UTC -9", -32400)},
        {"ALMT", TimeZone("ALMT", "Alma-Ata Time", "Asia/Almaty", "UTC +6", 21600)},
        {"ANAST", TimeZone("ANAST", "Anadyr Summer Time", "UTC+12:00", "UTC +12", 43200)},
        {"ANAT", TimeZone("ANAT", "Anadyr Time", "Asia/Anadyr", "UTC +12", 43200)},
        {"AQTT", TimeZone("AQTT", "Aqtobe Time", "Asia/Aqtobe", "UTC +5", 18000)},
        {"ART", TimeZone("ART", "Argentina Time", "UTC-03:00", "UTC -3", -10800)},
        {"AWDT", TimeZone("AWDT", "Australian Western Daylight Time", "UTC+09:00", "UTC +9", 32400)},
        {"AWST", TimeZone("AWST", "Australian Western Standard Time", "UTC+08:00", "UTC +8", 28800)},
        {"AZOST", TimeZone("AZOST", "Azores Summer Time", "Atlantic/Azores", "UTC +0", 0)},
        {"AZOT", TimeZone("AZOT", "Azores Time", "UTC-01:00", "UTC -1", -3600)},
        {"AZST", TimeZone("AZST", "Azerbaijan Summer Time", "UTC+05:00", "UTC +5", 18000)},
        {"AZT", TimeZone("AZT", "Azerbaijan Time", "Asia/Baku", "UTC +4", 14400)},
        {"AoE", TimeZone("AoE", "Anywhere on Earth", "UTC-12:00", "UTC -12", -43200)},
        {"B", TimeZone("B", "Bravo Time Zone", "UTC+02:00", "UTC +2", 7200)},
        {"BNT", TimeZone("BNT", "Brunei Darussalam Time", "Asia/Brunei", "UTC +8", 28800)},
        {"BOT", TimeZone("BOT", "Bolivia Time", "America/La_Paz", "UTC -4", -14400)},
        {"BRST", TimeZone("BRST", "Brasília Summer Time", "UTC-02:00", "UTC -2", -7200)},
        {"BRT", TimeZone("BRT", "Brasília Time", "UTC-03:00", "UTC -3", -10800)},
        {"BTT", TimeZone("BTT", "Bhutan Time", "Asia/Thimphu", "UTC +6", 21600)},
        {"C", TimeZone("C", "Charlie Time Zone", "UTC+03:00", "UTC +3", 10800)},
        {"CAST", TimeZone("CAST", "Casey Time", "Antarctica/Casey", "UTC +8", 28800)},
        {"CAT", TimeZone("CAT", "Central Africa Time", "UTC+02:00", "UTC +2", 7200)},
        {"CCT", TimeZone("CCT", "Cocos Islands Time", "Indian/Cocos", "UTC +6:30", 23400)},
        {"CEST", TimeZone("CEST", "Central European Summer Time", "UTC+02:00", "UTC +2", 7200)},
        {"CET", TimeZone("CET", "Central European Time", "UTC+01:00", "UTC +1", 3600)},
        {"CHADT", TimeZone("CHADT", "Chatham Island Daylight Time", "", "UTC +13:45", 49500)},
        {"CHAST", TimeZone("CHAST", "Chatham Island Standard Time", "Pacific/Chatham", "UTC +12:45", 45900)},
        {"CHOST", TimeZone("CHOST", "Choibalsan Summer Time", "UTC+09:00", "UTC +9", 32400)},
        {"CHOT", TimeZone("CHOT", "Choibalsan Time", "Asia/Choibalsan", "UTC +8", 28800)},
        {"CHUT", TimeZone("CHUT", "Chuuk Time", "Pacific/Chuuk", "UTC +10", 36000)},
        {"CIDST", TimeZone("CIDST", "Cayman Islands Daylight Saving Time", "UTC-04:00", "UTC -4", -14400)},
        {"CIST", TimeZone("CIST", "Cayman Islands Standard Time", "America/Cayman", "UTC -5", -18000)},
        {"CKT", TimeZone("CKT", "Cook Island Time", "Pacific/Rarotonga", "UTC -10", -36000)},
        {"CLST", TimeZone("CLST", "Chile Summer Time", "UTC-03:00", "UTC -3", -10800)},
        {"CLT", TimeZone("CLT", "Chile Standard Time", "UTC-04:00", "UTC -4", -14400)},
        {"COT", TimeZone("COT", "Colombia Time", "America/Bogota", "UTC -5", -18000)},
        {"CVT", TimeZone("CVT", "Cape Verde Time", "Atlantic/Cape_Verde", "UTC -1", -3600)},
        {"CXT", TimeZone("CXT", "Christmas Island Time", "Indian/Christmas", "UTC +7", 25200)},
        {"ChST", TimeZone("ChST", "Chamorro Standard Time", "Pacific/Saipan", "UTC +10", 36000)},
        {"D", TimeZone("D", "Delta Time Zone", "UTC+04:00", "UTC +4", 14400)},
        {"DAVT", TimeZone("DAVT", "Davis Time", "Antarctica/Davis", "UTC +7", 25200)},
        {"DDUT", TimeZone("DDUT", "Dumont-d'Urville Time", "UTC+10:00", "UTC +10", 36000)},
        {"E", TimeZone("E", "Echo Time Zone", "UTC+05:00", "UTC +5", 18000)},
        {"EASST", TimeZone("EASST", "Easter Island Summer Time", "UTC-05:00", "UTC -5", -18000)},
        {"EAST", TimeZone("EAST", "Easter Island Standard Time", "UTC-06:00", "UTC -6", -21600)},
        {"EAT", TimeZone("EAT", "Eastern Africa Time", "UTC+03:00", "UTC +3", 10800)},
        {"ECT", TimeZone("ECT", "Ecuador Time", "UTC-05:00", "UTC -5", -18000)},
        {"EDT", TimeZone("EDT", "Eastern Daylight Time", "UTC-04:00", "UTC -4", -14400)},
        {"EEST", TimeZone("EEST", "Eastern European Summer Time", "UTC+03:00", "UTC +3", 10800)},
        {"EET", TimeZone("EET", "Eastern European Time", "UTC+02:00", "UTC +2", 7200)},
        {"EGST", TimeZone("EGST", "Eastern Greenland Summer Time", "UTC+00:00", "UTC +0", 0)},
        {"EGT", TimeZone("EGT", "East Greenland Time", "UTC-01:00", "UTC -1", -3600)},
        {"EST", TimeZone("EST", "Eastern Standard Time", "UTC-05:00", "UTC -5", -18000)},
        {"F", TimeZone("F", "Foxtrot Time Zone", "UTC+06:00", "UTC +6", 21600)},
        {"FET", TimeZone("FET", "Further-Eastern European Time", "UTC+03:00", "UTC +3", 10800)},
        {"FJST", TimeZone("FJST", "Fiji Summer Time", "UTC+13:00", "UTC +13", 46800)},
        {"FJT", TimeZone("FJT", "Fiji Time", "Pacific/Fiji", "UTC +12", 43200)},
        {"FKST", TimeZone("FKST", "Falkland Islands Summer Time", "UTC-03:00", "UTC -3", -10800)},
        {"FKT", TimeZone("FKT", "Falkland Island Time", "UTC-04:00", "UTC -4", -14400)},
        {"FNT", TimeZone("FNT", "Fernando de Noronha Time", "America/Noronha", "UTC -2", -7200)},
        {"G", TimeZone("G", "Golf Time Zone", "UTC+07:00", "UTC +7", 25200)},
        {"GALT", TimeZone("GALT", "Galapagos Time", "Pacific/Galapagos", "UTC -6", -21600)},
        {"GAMT", TimeZone("GAMT", "Gambier Time", "Pacific/Gambier", "UTC -9", -32400)},
        {"GET", TimeZone("GET", "Georgia Standard Time", "Asia/Tbilisi", "UTC +4", 14400)},
        {"GFT", TimeZone("GFT", "French Guiana Time", "America/Cayenne", "UTC -3", -10800)},
        {"GILT", TimeZone("GILT", "Gilbert Island Time", "Pacific/Tarawa", "UTC +12", 43200)},
        {"GMT", TimeZone("GMT", "Greenwich Mean Time", "UTC+00:00", "UTC +0", 0)},
        {"GYT", TimeZone("GYT", "Guyana Time", "America/Guyana", "UTC -4", -14400)},
        {"H", TimeZone("H", "Hotel Time Zone", "UTC+08:00", "UTC +8", 28800)},
        {"HDT", TimeZone("HDT", "Hawaii-Aleutian Daylight Time", "UTC-09:00", "UTC -9", -32400)},
        {"HKT", TimeZone("HKT", "Hong Kong Time", "UTC+08:00", "UTC +8", 28800)},
        {"HOVST", TimeZone("HOVST", "Hovd Summer Time", "UTC+08:00", "UTC +8", 28800)},
        {"HOVT", TimeZone("HOVT", "Hovd Time", "Asia/Hovd", "UTC +7", 25200)},
        {"HST", TimeZone("HST", "Hawaii Standard Time", "Pacific/Honolulu", "UTC -10", -36000)},
        {"I", TimeZone("I", "India Time Zone", "UTC+09:00", "UTC +9", 32400)},
        {"ICT", TimeZone("ICT", "Indochina Time", "UTC+07:00", "UTC +7", 25200)},
        {"IDT", TimeZone("IDT", "Israel Daylight Time", "UTC+03:00", "UTC +3", 10800)},
        {"IOT", TimeZone("IOT", "Indian Chagos Time", "Indian/Chagos", "UTC +6", 21600)},
        {"IRDT", TimeZone("IRDT", "Iran Daylight Time", "Asia/Tehran", "UTC +4:30", 16200)},
        {"IRKST", TimeZone("IRKST", "Irkutsk Summer Time", "UTC+09:00", "UTC +9", 32400)},
        {"IRKT", TimeZone("IRKT", "Irkutsk Time", "Asia/Irkutsk", "UTC +8", 28800)},
        {"IRST", TimeZone("IRST", "Iran Standard Time", "UTC+03:30", "UTC +3:30", 12600)},
        {"JST", TimeZone("JST", "Japan Standard Time", "Asia/Tokyo", "UTC +9", 32400)},
        {"K", TimeZone("K", "Kilo Time Zone", "UTC+10:00", "UTC +10", 36000)},
        {"KGT", TimeZone("KGT", "Kyrgyzstan Time", "Asia/Bishkek", "UTC +6", 21600)},
        {"KOST", TimeZone("KOST", "Kosrae Time", "Pacific/Kosrae", "UTC +11", 39600)},
        {"KRAST", TimeZone("KRAST", "Krasnoyarsk Summer Time", "UTC+08:00", "UTC +8", 28800)},
        {"KRAT", TimeZone("KRAT", "Krasnoyarsk Time", "Asia/Krasnoyarsk", "UTC +7", 25200)},
        {"KST", TimeZone("KST", "Korea Standard Time", "Asia/Pyongyang", "UTC +9", 32400)},
        {"KUYT", TimeZone("KUYT", "Kuybyshev Time", "UTC+04:00", "UTC +4", 14400)},
        {"L", TimeZone("L", "Lima Time Zone", "UTC+11:00", "UTC +11", 39600)},
        {"LHDT", TimeZone("LHDT", "Lord Howe Daylight Time", "UTC+11:00", "UTC +11", 39600)},
        {"LHST", TimeZone("LHST", "Lord Howe Standard Time", "Australia/Lord_Howe", "UTC +10:30", 37800)},
        {"LINT", TimeZone("LINT", "Line Islands Time", "UTC+14:00", "UTC +14", 50400)},
        {"M", TimeZone("M", "Mike Time Zone", "UTC+12:00", "UTC +12", 43200)},
        {"MAGST", TimeZone("MAGST", "Magadan Summer Time", "UTC+12:00", "UTC +12", 43200)},
        {"MAGT", TimeZone("MAGT", "Magadan Time", "Asia/Magadan", "UTC +11", 39600)},
        {"MART", TimeZone("MART", "Marquesas Time", "Pacific/Marquesas", "UTC -9:30", -34200)},
        {"MAWT", TimeZone("MAWT", "Mawson Time", "Antarctica/Mawson", "UTC +5", 18000)},
        {"MDT", TimeZone("MDT", "Mountain Daylight Time", "UTC-06:00", "UTC -6", -21600)},
        {"MHT", TimeZone("MHT", "Marshall Islands Time", "UTC+12:00", "UTC +12", 43200)},
        {"MMT", TimeZone("MMT", "Myanmar Time", "Asia/Yangon", "UTC +6:30", 23400)},
        {"MSD", TimeZone("MSD", "Moscow Daylight Time", "UTC+04:00", "UTC +4", 14400)},
        {"MSK", TimeZone("MSK", "Moscow Standard Time", "Europe/Moscow", "UTC +3", 10800)},
        {"MST", TimeZone("MST", "Mountain Standard Time", "UTC-07:00", "UTC -7", -25200)},
        {"MUT", TimeZone("MUT", "Mauritius Time", "Indian/Mauritius", "UTC +4", 14400)},
        {"MVT", TimeZone("MVT", "Maldives Time", "Indian/Maldives", "UTC +5", 18000)},
        {"MYT", TimeZone("MYT", "Malaysia Time", "Asia/Kuala_Lumpur", "UTC +8", 28800)},
        {"N", TimeZone("N", "November Time Zone", "UTC-01:00", "UTC -1", -3600)},
        {"NCT", TimeZone("NCT", "New Caledonia Time", "Pacific/Noumea", "UTC +11", 39600)},
        {"NDT", TimeZone("NDT", "Newfoundland Daylight Time", "", "UTC -2:30", -9000)},
        {"NFDT", TimeZone("NFDT", "Norfolk Daylight Time", "UTC+12:00", "UTC +12", 43200)},
        {"NFT", TimeZone("NFT", "Norfolk Time", "Pacific/Norfolk", "UTC +11", 39600)},
        {"NOVST", TimeZone("NOVST", "Novosibirsk Summer Time", "UTC+07:00", "UTC +7", 25200)},
        {"NOVT", TimeZone("NOVT", "Novosibirsk Time", "Asia/Novosibirsk", "UTC +7", 25200)},
        {"NPT", TimeZone("NPT", "Nepal Time", "Asia/Kathmandu", "UTC +5:45", 20700)},
        {"NRT", TimeZone("NRT", "Nauru Time", "Pacific/Nauru", "UTC +12", 43200)},
        {"NST", TimeZone("NST", "Newfoundland Standard Time", "UTC-03:30", "UTC -3:30", -12600)},
        {"NUT", TimeZone("NUT", "Niue Time", "Pacific/Niue", "UTC -11", -39600)},
        {"NZDT", TimeZone("NZDT", "New Zealand Daylight Time", "UTC+13:00", "UTC +13", 46800)},
        {"NZST", TimeZone("NZST", "New Zealand Standard Time", "UTC+12:00", "UTC +12", 43200)},
        {"O", TimeZone("O", "Oscar Time Zone", "UTC-02:00", "UTC -2", -7200)},
        {"OMSST", TimeZone("OMSST", "Omsk Summer Time", "UTC+07:00", "UTC +7", 25200)},
        {"OMST", TimeZone("OMST", "Omsk Standard Time", "Asia/Omsk", "UTC +6", 21600)},
        {"ORAT", TimeZone("ORAT", "Oral Time", "Asia/Oral", "UTC +5", 18000)},
        {"P", TimeZone("P", "Papa Time Zone", "UTC-03:00", "UTC -3", -10800)},
        {"PDT", TimeZone("PDT", "Pacific Daylight Time", "UTC-07:00", "UTC -7", -25200)},
        {"PET", TimeZone("PET", "Peru Time", "America/Lima", "UTC -5", -18000)},
        {"PETST", TimeZone("PETST", "Kamchatka Summer Time", "UTC+12:00", "UTC +12", 43200)},
        {"PETT", TimeZone("PETT", "Kamchatka Time", "Asia/Kamchatka", "UTC +12", 43200)},
        {"PGT", TimeZone("PGT", "Papua New Guinea Time", "Pacific/Port_Moresby", "UTC +10", 36000)},
        {"PHOT", TimeZone("PHOT", "Phoenix Island Time", "UTC+13:00", "UTC +13", 46800)},
        {"PHT", TimeZone("PHT", "Philippine Time", "Asia/Manila", "UTC +8", 28800)},
        {"PKT", TimeZone("PKT", "Pakistan Standard Time", "Asia/Karachi", "UTC +5", 18000)},
        {"PMDT", TimeZone("PMDT", "Pierre & Miquelon Daylight Time", "UTC-02:00", "UTC -2", -7200)},
        {"PMST", TimeZone("PMST", "Pierre & Miquelon Standard Time", "UTC-03:00", "UTC -3", -10800)},
        {"PONT", TimeZone("PONT", "Pohnpei Standard Time", "Pacific/Pohnpei", "UTC +11", 39600)},
        {"PWT", TimeZone("PWT", "Palau Time", "Pacific/Palau", "UTC +9", 32400)},
        {"PYST", TimeZone("PYST", "Paraguay Summer Time", "UTC-03:00", "UTC -3", -10800)},
        {"Q", TimeZone("Q", "Quebec Time Zone", "America/Blanc-Sablon", "UTC -4", -14400)},
        {"QYZT", TimeZone("QYZT", "Qyzylorda Time", "UTC+06:00", "UTC +6", 21600)},
        {"R", TimeZone("R", "Romeo Time Zone", "UTC-05:00", "UTC -5", -18000)},
        {"RET", TimeZone("RET", "Reunion Time", "Indian/Reunion", "UTC +4", 14400)},
        {"ROTT", TimeZone("ROTT", "Rothera Time", "Antarctica/Rothera", "UTC -3", -10800)},
        {"S", TimeZone("S", "Sierra Time Zone", "UTC-06:00", "UTC -6", -21600)},
        {"SAKT", TimeZone("SAKT", "Sakhalin Time", "Asia/Sakhalin", "UTC +11", 39600)},
        {"SAMT", TimeZone("SAMT", "Samara Time", "Europe/Samara", "UTC +4", 14400)},
        {"SAST", TimeZone("SAST", "South Africa Standard Time", "UTC+02:00", "UTC +2", 7200)},
        {"SBT", TimeZone("SBT", "Solomon Islands Time", "Pacific/Guadalcanal", "UTC +11", 39600)},
        {"SCT", TimeZone("SCT", "Seychelles Time", "Indian/Mahe", "UTC +4", 14400)},
        {"SGT", TimeZone("SGT", "Singapore Time", "Asia/Singapore", "UTC +8", 28800)},
        {"SRET", TimeZone("SRET", "Srednekolymsk Time", "Asia/Srednekolymsk", "UTC +11", 39600)},
        {"SRT", TimeZone("SRT", "Suriname Time", "America/Paramaribo", "UTC -3", -10800)},
        {"SST", TimeZone("SST", "Samoa Standard Time", "UTC-11:00", "UTC -11", -39600)},
        {"SYOT", TimeZone("SYOT", "Syowa Time", "Antarctica/Syowa", "UTC +3", 10800)},
        {"T", TimeZone("T", "Tango Time Zone", "UTC-07:00", "UTC -7", -25200)},
        {"TAHT", TimeZone("TAHT", "Tahiti Time", "Pacific/Tahiti", "UTC -10", -36000)},
        {"TFT", TimeZone("TFT", "French Southern and Antarctic Time", "UTC+05:00", "UTC +5", 18000)},
        {"TJT", TimeZone("TJT", "Tajikistan Time", "Asia/Dushanbe", "UTC +5", 18000)},
        {"TKT", TimeZone("TKT", "Tokelau Time", "Pacific/Fakaofo", "UTC +13", 46800)},
        {"TLT", TimeZone("TLT", "East Timor Time", "Asia/Dili", "UTC +9", 32400)},
        {"TMT", TimeZone("TMT", "Turkmenistan Time", "Asia/Ashgabat", "UTC +5", 18000)},
        {"TOST", TimeZone("TOST", "Tonga Summer Time", "UTC+14:00", "UTC +14", 50400)},
        {"TOT", TimeZone("TOT", "Tonga Time", "Pacific/Tongatapu", "UTC +13", 46800)},
        {"TRT", TimeZone("TRT", "Turkey Time", "Europe/Istanbul", "UTC +3", 10800)},
        {"TVT", TimeZone("TVT", "Tuvalu Time", "Pacific/Funafuti", "UTC +12", 43200)},
        {"U", TimeZone("U", "Uniform Time Zone", "UTC-08:00", "UTC -8", -28800)},
        {"ULAST", TimeZone("ULAST", "Ulaanbaatar Summer Time", "UTC+09:00", "UTC +9", 32400)},
        {"ULAT", TimeZone("ULAT", "Ulaanbaatar Time", "Asia/Ulaanbaatar", "UTC +8", 28800)},
        {"UYST", TimeZone("UYST", "Uruguay Summer Time", "UTC-02:00", "UTC -2", -7200)},
        {"UYT", TimeZone("UYT", "Uruguay Time", "America/Montevideo", "UTC -3", -10800)},
        {"UZT", TimeZone("UZT", "Uzbekistan Time", "Asia/Tashkent", "UTC +5", 18000)},
        {"V", TimeZone("V", "Victor Time Zone", "UTC-09:00", "UTC -9", -32400)},
        {"VET", TimeZone("VET", "Venezuelan Standard Time", "America/Caracas", "UTC -4", -14400)},
        {"VLAST", TimeZone("VLAST", "Vladivostok Summer Time", "UTC+11:00", "UTC +11", 39600)},
        {"VLAT", TimeZone("VLAT", "Vladivostok Time", "Asia/Vladivostok", "UTC +10", 36000)},
        {"VOST", TimeZone("VOST", "Vostok Time", "Antarctica/Vostok", "UTC +6", 21600)},
        {"VUT", TimeZone("VUT", "Vanuatu Time", "Pacific/Efate", "UTC +11", 39600)},
        {"W", TimeZone("W", "Whiskey Time Zone", "UTC-10:00", "UTC -10", -36000)},
        {"WAKT", TimeZone("WAKT", "Wake Time", "Pacific/Wake", "UTC +12", 43200)},
        {"WARST", TimeZone("WARST", "Western Argentine Summer Time", "UTC-03:00", "UTC -3", -10800)},
        {"WAST", TimeZone("WAST", "West Africa Summer Time", "UTC+02:00", "UTC +2", 7200)},
        {"WAT", TimeZone("WAT", "West Africa Time", "UTC+01:00", "UTC +1", 3600)},
        {"WEST", TimeZone("WEST", "Western European Summer Time", "UTC+01:00", "UTC +1", 3600)},
        {"WET", TimeZone("WET", "Western European Time", "UTC+00:00", "UTC +0", 0)},
        {"WFT", TimeZone("WFT", "Wallis and Futuna Time", "Pacific/Wallis", "UTC +12", 43200)},
        {"WGST", TimeZone("WGST", "Western Greenland Summer Time", "UTC-02:00", "UTC -2", -7200)},
        {"WGT", TimeZone("WGT", "West Greenland Time", "UTC-03:00", "UTC -3", -10800)},
        {"WIB", TimeZone("WIB", "Western Indonesian Time", "UTC+07:00", "UTC +7", 25200)},
        {"WIT", TimeZone("WIT", "Eastern Indonesian Time", "UTC+09:00", "UTC +9", 32400)},
        {"WITA", TimeZone("WITA", "Central Indonesian Time", "UTC+08:00", "UTC +8", 28800)},
        {"WT", TimeZone("WT", "Western Sahara Standard Time", "UTC+00:00", "UTC +0", 0)},
        {"X", TimeZone("X", "X-ray Time Zone", "UTC-11:00", "UTC -11", -39600)},
        {"Y", TimeZone("Y", "Yankee Time Zone", "UTC-12:00", "UTC -12", -43200)},
        {"YAKST", TimeZone("YAKST", "Yakutsk Summer Time", "UTC+10:00", "UTC +10", 36000)},
        {"YAKT", TimeZone("YAKT", "Yakutsk Time", "Asia/Yakutsk", "UTC +9", 32400)},
        {"YAPT", TimeZone("YAPT", "Yap Time", "Pacific/Chuuk", "UTC +10", 36000)},
        {"YEKST", TimeZone("YEKST", "Yekaterinburg Summer Time", "UTC+06:00", "UTC +6", 21600)},
        {"YEKT", TimeZone("YEKT", "Yekaterinburg Time", "Asia/Yekaterinburg", "UTC +5", 18000)},
        {"Z", TimeZone("Z", "Zulu Time Zone", "UTC+00:00", "UTC +0", 0)},
    };
    return zones.value(code);
}

QString DateTime::timeZoneRegExp()
{
    // see timezones.js
    static QStringList tzs = {"YEKST", "YAKST", "WARST", "VLAST", "ULAST", "PETST", "OMSST", "NOVST",
                              "MAGST", "KRAST", "IRKST", "HOVST", "EASST", "CIDST", "CHOST", "CHAST", "CHADT",
                              "AZOST", "ANAST", "ACWST", "YEKT", "YAPT", "YAKT", "WITA", "WGST", "WEST",
                              "WAST", "WAKT", "VOST", "VLAT", "UYST", "ULAT", "TOST", "TAHT", "SYOT", "SRET",
                              "SAST", "SAMT", "SAKT", "ROTT", "QYZT", "PYST", "PONT", "PMST", "PMDT", "PHOT",
                              "PETT", "ORAT", "OMST", "NZST", "NZDT", "NOVT", "NFDT", "MAWT", "MART", "MAGT",
                              "LINT", "LHST", "LHDT", "KUYT", "KRAT", "KOST", "IRST", "IRKT", "IRDT", "HOVT",
                              "GILT", "GAMT", "GALT", "FKST", "FJST", "EGST", "EEST", "EAST", "DDUT", "DAVT",
                              "ChST", "CLST", "CIST", "CHUT", "CHOT", "CEST", "CAST", "BRST", "AZST", "AZOT",
                              "AWST", "AWDT", "AQTT", "ANAT", "ALMT", "AKST", "AKDT", "AEST", "AEDT", "ACST",
                              "ACDT", "WIT", "WIB", "WGT", "WFT", "WET", "WAT", "VUT", "VET", "UZT", "UYT",
                              "UTC", "TVT", "TRT", "TOT", "TMT", "TLT", "TKT", "TJT", "TFT", "SST", "SRT",
                              "SGT", "SCT", "SBT", "RET", "PWT", "PKT", "PHT", "PGT", "PET", "PDT", "NUT",
                              "NST", "NRT", "NPT", "NFT", "NDT", "NCT", "MYT", "MVT", "MUT", "MST", "MSK",
                              "MSD", "MMT", "MHT", "MDT", "KST", "KGT", "JST", "IOT", "IDT", "ICT", "HST",
                              "HKT", "HDT", "GYT", "GMT", "GFT", "GET", "FNT", "FKT", "FJT", "FET", "EST",
                              "EGT", "EET", "EDT", "ECT", "EAT", "CXT", "CVT", "COT", "CLT", "CKT", "CET",
                              "CCT", "CAT", "BTT", "BRT", "BOT", "BNT", "AoE", "AZT", "ART", "AFT", "AET",
                              "WT", "PT", "MT", "ET", "CT", "AT", "Z", "Y", "X", "W", "V", "U", "T", "S", "R",
                              "Q", "P", "O", "N", "M", "L", "K", "I", "H", "G", "F", "E", "D", "C", "B", "A"};

    QString tzNumeric = "[+][0-9]{4}";
    QString tzRtz = "RTZ\\s[0-9]+\\s[(]зима[)]";

    return group(tzs.join("|") + "|" + tzNumeric + "|" + tzRtz);
}

QTimeZone DateTime::parseTimeZone(const QString& timeZoneCode) {

    QRegularExpression tzNumeric("[+]([0-9]{2})([0-9]{2})");
    QRegularExpression tzRtz("RTZ\\s([0-9]+)\\s[(]зима[)]");

    QRegularExpressionMatch m = tzNumeric.match(timeZoneCode);
    if (m.hasMatch()) {
        int hours = m.captured(1).toInt();
        int minutes = m.captured(2).toInt();
        return QTimeZone(hours * 3600 + minutes * 60);
    }
    m = tzRtz.match(timeZoneCode);
    if (m.hasMatch()) {
        int rtz = m.captured(1).toInt();
        return QTimeZone((rtz + 1) * 3600);
    }

    TimeZone timeZoneData = DateTime::timeZone(timeZoneCode);
    if (timeZoneData.isValid()) {
        QTimeZone timeZone =  QTimeZone(timeZoneData.ianaId());
        if (!timeZone.isValid()) {
            timeZone = QTimeZone(timeZoneData.offset());
        }
        return timeZone;
    }
    return QTimeZone();
}

QString DateTime::parseTimeZone(const QString& s, QDateTime& dateTime, bool* hasTimeZone) {

    QRegularExpression rx(head("\\s?" + timeZoneRegExp()));
    QRegularExpressionMatch m = rx.match(s);
    *hasTimeZone = m.hasMatch();
    if (m.hasMatch()) {
        QTimeZone timeZone = parseTimeZone(m.captured(1));
        if (timeZone.isValid()) {
            dateTime = QDateTime(dateTime.date(), dateTime.time(), timeZone);
        } else {
            qDebug() << "invalid timezone" << m.captured();
            *hasTimeZone = false;
        }
        return s.mid(m.capturedLength());
    }
    return s;
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
                                            Qt::SystemLocaleShortDate,
                                            Qt::SystemLocaleLongDate,
                                            Qt::DefaultLocaleShortDate,
                                            Qt::DefaultLocaleLongDate,
                                            Qt::RFC2822Date,
                                            Qt::ISODateWithMs};

    QStringList dateFormatNames = {"TextDate",
                                   "ISODate",
                                   "SystemLocaleShortDate",
                                   "SystemLocaleLongDate",
                                   "DefaultLocaleShortDate",
                                   "DefaultLocaleLongDate",
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
            stream.setCodec(QTextCodec::codecForName("UTF-8"));

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

    QList<QByteArray> zones = QTimeZone::availableTimeZoneIds();

    QDir dir(".");
    dir.mkpath("data");
    dir.cd("data");

    QString filePath = dir.filePath("zones.txt");

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "cannot open" << filePath;
        return;
    }
    foreach (const QByteArray& zone, zones) {
        file.write(zone);
        file.write("\n");
    }
    file.close();

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
    stream.setCodec(QTextCodec::codecForName("UTF-8"));

    for (int i=0;i<500;i++) {
        int a = rand() % 100;
        int b = rand() % 100;
        if (b > a) {
            stream << a << " " << b << " " << number(a,b) << "\n";
        }
    }

    qDebug() << filePath << "writen";
}
