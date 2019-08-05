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

QString number(int min, int max) {
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
            if (min1 == 0) {
                result << range(min2,9);
            }
        }
        if ((max1 - min1) > 1) {
            result << range(up ? min1 : min1 + 1, down ? max1 : max1 - 1) + range(0,9);
        }
        if (!down) {
            result << range(max1,max1) + range(0,max2);
        }
    }
    return "(" + result.join("|") + ")";
}


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

QString group(const QString& s) {
    return "(" + s + ")";
}

QString DateTime::regExpDateTime(FormatDateTime format) {
    return regExp(TypeDate, format, FormatDateUndefined, FormatTimeUndefined);
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
    QString yearDot = "(?:\\sг.)?";

    if (type == TypeDateTime){
        if (formatDateTime != FormatDateTimeUndefined) {

        } else {
            return regExp(TypeDate, formatDateTime, formatDate, formatTime) +
                    notDigit +
                    regExp(TypeTime, formatDateTime, formatDate, formatTime);
        }
    } else if (type == TypeDate) {
        static QMap<FormatDate, QString> exps = {
            {FormatDateYYYYMMDD, group("[0-9]{4}") + notDigit + number(1,12) + notDigit + number(1,31)},
            {FormatDateDDMMYY, number(1,31) + notDigit + number(1,12) + notDigit + group("[0-9]{2,4}") + yearDot },
            {FormatDateRuDDMonYY, number(1,31) + notDigit + group(ruMonthsShort().join("|")) + notDigit + "[0-9]{2,4}" + yearDot },
            {FormatDateRuDDMonthYY, number(1,31) + notDigit + group(ruMonthsLong().join("|")) + notDigit + "[0-9]{2,4}" + yearDot },
            {FormatDateEnDDMonYY, number(1,31) + notDigit + group(enMonthsShort().join("|")) + notDigit + "[0-9]{2,4}" },
            {FormatDateEnDDMonthYY, number(1,31) + notDigit + group(enMonthsLong().join("|")) + notDigit + "[0-9]{2,4}" }
        };
        return exps[formatDate];
    } else if (type == TypeTime) {
        static QMap<FormatTime, QString> exps = {
            {FormatTimeHM, group(number(0,24) + ":" + number(0,60)) + "(\\sAM|\\sPM)?" },
            {FormatTimeHMS, group(number(0,24) + ":" + number(0,60) + ":" + number(0,60)) + "(\\sAM|\\sPM)?"},
            {FormatTimeHMSMS, group(number(0,24) + ":" + number(0,60) + ":" + number(0,60) + "[.][0-9]{1,3}") + "(\\sAM|\\sPM)?"},
        };
        return exps[formatTime];
    }
}

bool dateTimeMaybe(const QString& s) {
    QRegularExpression fourDigitGroups("[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+");
    return fourDigitGroups.match(s).hasMatch() && s.indexOf(":") > 0;
}

bool timeMaybe(const QString& s) {
    return s.indexOf(":") > 0;
}

bool dateMaybe(const QString& s) {
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
        exps << QRegularExpression(regExpTime(format), QRegularExpression::CaseInsensitiveOption);
    }
    for(int i=0;i<formats.size();i++) {
        QRegularExpression rx = exps[i];
        QRegularExpressionMatch m = rx.match(head(s + "\\s?"));
        if (m.hasMatch()) {
            if (m.lastCapturedIndex() == 2) {
                time = parseAmPmTime(m.captured(1),m.captured(2),formats_[i]);
            } else {
                time = QTime::fromString(m.captured(1),formats_[i]);
            }
            return s.mid(m.capturedLength());
        }
    }
    return s;
}

QString DateTime::parseDate(const QString& s, QDate& date, int minYear) {

    QList<FormatDate> formats = {FormatDateYYYYMMDD,
                                 FormatDateDDMMYY,
                                 FormatDateRuDDMonYY,
                                 FormatDateRuDDMonthYY,
                                 FormatDateEnDDMonYY,
                                 FormatDateEnDDMonthYY};

    QList<QRegularExpression> exps;
    foreach (FormatDate format, formats) {
        exps << QRegularExpression(regExpDate(format), QRegularExpression::CaseInsensitiveOption);
    }

    for(int i=0;i<formats.size();i++) {
        FormatDate format = formats[i];
        QRegularExpression rx = exps[i];
        QRegularExpressionMatch m = rx.match(head(s + "\\s?"));

        if (m.hasMatch()) {
            QString year_ = format == FormatDateYYYYMMDD ? m.captured(1) : m.captured(3);
            int year = year_.toInt();
            if (year_.size() < 3) {
                year = twoDigitYear(year, minYear);
            }
            int month = -1;
            if (format == FormatDateYYYYMMDD || format == FormatDateDDMMYY) {
                month = m.captured(2).toInt();
            } else if (format == FormatDateRuDDMonYY) {
                month = ruMonthsShort().indexOf(m.captured(2)) + 1;
            } else if (format == FormatDateRuDDMonthYY) {
                month = ruMonthsLong().indexOf(m.captured(2)) + 1;
            } else if (format == FormatDateEnDDMonYY) {
                month = enMonthsShort().indexOf(m.captured(2)) + 1;
            } else if (format == FormatDateEnDDMonthYY) {
                month = enMonthsLong().indexOf(m.captured(2)) + 1;
            }
            int day = format == FormatDateYYYYMMDD ? m.captured(3).toInt() : m.captured(1).toInt();
            date = QDate(year,month,day);
            return s.mid(m.captured());
        }
    }
    return s;
}

bool DateTime::parse(Type type, const QString& s, QDate& date, QTime& time,
                     QDateTime& dateTime, int minYear, bool inLocalTime, bool outLocalTime) {

    if (type == TypeUnknown) {
        QDate date_;
        QTime time_;
        QDateTime dateTime_;
        if (parse(TypeDateTime, s, date_, time_, dateTime_, minYear, inLocalTime, outLocalTime)) {
            dateTime = dateTime_;
            return true;
        }
        if (parse(TypeTime, s, date_, time_, dateTime_, minYear, inLocalTime, outLocalTime)) {
            time = time_;
            return true;
        }
        if (parse(TypeDate, s, date_, time_, dateTime_, minYear, inLocalTime, outLocalTime)) {
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
        QString s_ = parseTime(s_, time_);
        if (!s_.isEmpty() || !time_.isValid()) {
            return false;
        }
        time = time_;
        return true;

    } else if (type == TypeDateTime) {

        if (!dateTimeMaybe(s)) {
            return false;
        }

        QDate date_;
        QTime time_;
        QString s_ = parseDate(s_, date_, minYear);
        if (!date_.isValid()) {
            return false;
        }
        s_ = parseTime(s_, time_);
        if (!time_.isValid()) {
            return false;
        }
        bool hasTimeZone;
        QDateTime dateTime_(date_,time_);
        s_ = parseTimeZone(s, dateTime_, &hasTimeZone);
        if (!s_.isEmpty()) {
            return false;
        }
        if (hasTimeZone) {
            dateTime = outLocalTime ? dateTime_.toLocalTime() : dateTime.toUTC();
        } else {
            dateTime_ = QDateTime(date_, time_, inLocalTime ? Qt::LocalTime : Qt::UTC);
            dateTime = outLocalTime ? dateTime_.toLocalTime() : dateTime.toUTC();
        }
        return true;
    }

    return false;
}

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
    static QStringList tzs = {"A", "ACDT", "ACST", "ACT", "ACT", "ACWST",
    "ADT", "ADT", "AEDT", "AEST", "AET", "AFT", "AKDT", "AKST", "ALMT", "AMST",
    "AMST", "AMT", "AMT", "ANAST", "ANAT", "AQTT", "ART", "AST", "AST", "AT",
    "AWDT", "AWST", "AZOST", "AZOT", "AZST", "AZT", "AoE", "B", "BNT", "BOT",
    "BRST", "BRT", "BST", "BST", "BST", "BTT", "C", "CAST", "CAT", "CCT",
    "CDT", "CDT", "CEST", "CET", "CHADT", "CHAST", "CHOST", "CHOT", "CHUT",
    "CIDST", "CIST", "CKT", "CLST", "CLT", "COT", "CST", "CST", "CST", "CT",
    "CVT", "CXT", "ChST", "D", "DAVT", "DDUT", "E", "EASST", "EAST", "EAT",
    "ECT", "EDT", "EEST", "EET", "EGST", "EGT", "EST", "ET", "F", "FET",
    "FJST", "FJT", "FKST", "FKT", "FNT", "G", "GALT", "GAMT", "GET", "GFT",
    "GILT", "GMT", "GST", "GST", "GYT", "H", "HDT", "HKT", "HOVST", "HOVT",
    "HST", "I", "ICT", "IDT", "IOT", "IRDT", "IRKST", "IRKT", "IRST", "IST",
    "IST", "IST", "JST", "K", "KGT", "KOST", "KRAST", "KRAT", "KST", "KUYT",
    "L", "LHDT", "LHST", "LINT", "M", "MAGST", "MAGT", "MART", "MAWT", "MDT",
    "MHT", "MMT", "MSD", "MSK", "MST", "MT", "MUT", "MVT", "MYT", "N", "NCT",
    "NDT", "NFDT", "NFT", "NOVST", "NOVT", "NPT", "NRT", "NST", "NUT", "NZDT",
    "NZST", "O", "OMSST", "OMST", "ORAT", "P", "PDT", "PET", "PETST", "PETT",
    "PGT", "PHOT", "PHT", "PKT", "PMDT", "PMST", "PONT", "PST", "PST", "PT",
    "PWT", "PYST", "PYT", "PYT", "Q", "QYZT", "R", "RET", "ROTT", "S", "SAKT",
    "SAMT", "SAST", "SBT", "SCT", "SGT", "SRET", "SRT", "SST", "SYOT", "T",
    "TAHT", "TFT", "TJT", "TKT", "TLT", "TMT", "TOST", "TOT", "TRT", "TVT",
    "U", "ULAST", "ULAT", "UTC", "UYST", "UYT", "UZT", "V", "VET", "VLAST",
    "VLAT", "VOST", "VUT", "W", "WAKT", "WARST", "WAST", "WAT", "WEST", "WET",
    "WFT", "WGST", "WGT", "WIB", "WIT", "WITA", "WST", "WST", "WT", "X", "Y",
    "YAKST", "YAKT", "YAPT", "YEKST", "YEKT", "Z"};

    QString tzNumeric = "[+][0-9]{4}";
    QString tzRtz = "RTZ\\s[0-9]+\\s[(]зима[)]";

    return group(tzs.join("|") + "|" + tzNumeric + "|" + tzRtz);
}

QTimeZone DateTime::parseTimeZone(const QString& timeZone) {

    QRegularExpression tzNumeric("[+]([0-9]{2})([0-9]{2})");
    QRegularExpression tzRtz("RTZ\\s([0-9]+)\\s[(]зима[)]");

    QRegularExpressionMatch m = tzNumeric.match(timeZone);
    if (m.hasMatch()) {
        int hours = m.captured(1).toInt();
        int minutes = m.captured(2).toInt();
        return QTimeZone(hours * 3600 + minutes * 60);
    }
    m = tzRtz.match(timeZone);
    if (m.hasMatch()) {
        int rtz = m.captured(1).toInt();
        return QTimeZone((rtz + 1) * 3600);
    }

    TimeZone timeZone_ = DateTime::timeZone(timeZone);
    if (timeZone_.isValid()) {
        return QTimeZone(timeZone_.ianaId());
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

QStringList DateTime::ruMonthsShort() {
    return {"янв", "фев", "мар", "апр", "май", "июн", "июл", "авг", "сен", "окт", "ноя", "дек"};
}

QStringList DateTime::ruMonthsLong()
{
    return {"января","февраля","марта","апреля","мая","июня","июля","августа","сентября","октября","ноября","декабря"};
}

QStringList DateTime::enMonthsShort()
{
    return {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
}

QStringList DateTime::enMonthsLong() {
    return {"january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};
}

QStringList DateTime::ruWeekDaysShort()
{
    return {"пн", "вт", "ср", "чт", "пт", "сб", "вс"};
}

QList<QRegularExpression> DateTime::dateRegularExpressions()
{
    return {};
}

QList<QRegularExpression> DateTime::dateTimeRegularExpressions()
{
    return {};
}

QList<QRegularExpression> DateTime::timeRegularExpressions()
{
    return {};
}

#if 0

QRegularExpression DateTime::timeRegularExpression(DateTime::Format format)
{
    QString hm = "[0-9]{1,2}:[0-9]{2}";
    QString hms = hm + ":[0-9]{2}";

    if (format == Format5) {
        // 07.08.19 5:00 AM
        return QRegularExpression("^(" + hm + ")\\s(AM|PM)$");
    } else if (format == Format6) {
        // 07.08.19 09:08:15 PM
        return QRegularExpression("^(" + hms + ")\\s(AM|PM)$");
    }
    return QRegularExpression();
}

QRegularExpression DateTime::dateRegularExpression(Format format)
{
    if (format == FormatRuLocaleLong) {
        // 5 сентября 1974 г.
        // 5 сентября 1974
        // 5-сентября-1974
        QStringList months = ruMonthsLong();
        return QRegularExpression("^([0-9]{1,2})[\\s-](" + months.join("|") + ")[\\s-]([0-9]{4})(?:\\sг.)?$");
    } else if (format == FormatRuLocaleText) {
        // Чт май 28 1970
        // Чт май 28 1970 г.
        QStringList months = ruMonthsShort();
        QStringList weekDays = ruWeekDaysShort();
        return QRegularExpression("^(" + weekDays.join("|") + ")\\s(" + months.join("|") + ")\\s([0-9]{1,2})\\s([0-9]{4})(?:\\sг.)?$");
    /*} else if (format == Format1) {
        // 02.08.2019
        return QRegularExpression("^([0-9]{1,2})[.]([0-9]{1,2})[.]([0-9]{4})$");*/
    } else if (format == Format2) {
        // 02.08.19
        return QRegularExpression("^([0-9]{1,2})[.]([0-9]{1,2})[.]([0-9]{2})$");
    } else if (format == Format3) {
        // 6 авг 19
        // 6-авг-19
        QStringList months = ruMonthsShort();
        return QRegularExpression("^([0-9]{1,2})[\\s-](" + months.join("|") + ")[\\s-]([0-9]{2})$");
    } else if (format == Format4) {
        // 9 авг 2019
        // 9-авг-2019
        // 9-авг-2019 г.
        QStringList months = ruMonthsShort();
        return QRegularExpression("^([0-9]{1,2})[\\s-](" + months.join("|") + ")[\\s-]([0-9]{4})(?:\\sг.)?$");
    }

    return QRegularExpression();
}

QRegularExpression DateTime::dateTimeRegularExpression(Format format)
{
    QString hms = "[0-9]{1,2}:[0-9]{2}:[0-9]{2}";
    QString hm = "[0-9]{1,2}:[0-9]{2}";
    QString dotted_ddmmyy = "([0-9]{1,2})[./-]([0-9]{1,2})[./-]([0-9]{1,2})";

    if (format == FormatRuLocaleLong) {
        // 22 августа 2023 г. 19:35:14
        QStringList months = ruMonthsLong();
        return QRegularExpression("^([0-9]{1,2})[\\s-](" + months.join("|") + ")[\\s-]([0-9]{4})(?:\\sг.)?\\s(" + hms + ")$");
    } else if (format == FormatRuLocaleText) {
        // Сб дек 23 19:09:00 2006
        QStringList months = ruMonthsShort();
        QStringList weekDays = ruWeekDaysShort();
        return QRegularExpression("^(" + weekDays.join("|") + ")\\s(" + months.join("|") + ")\\s([0-9]{1,2})\\s(" + hms + ")\\s([0-9]{4})$");
    } else if (format == FormatRuLocaleShortTwoDigitYear) {
        // 05.08.19 3:00
        return QRegularExpression("^" + dotted_ddmmyy + "\\s(" + hm + ")$");
    } else if (format == Format5) {
        // 07.08.19 5:00 AM
        return QRegularExpression("^" + dotted_ddmmyy + "\\s(" + hm + ")\\s(AM|PM)$");
    }
    return QRegularExpression();
}

QDate DateTime::parseDate(const QString &s, int minYear)
{

    QRegularExpression rx;
    QRegularExpressionMatch m;

    rx = dateRegularExpression(FormatRuLocaleLong);
    m = rx.match(s);
    if (m.hasMatch()) {
        QStringList months = ruMonthsLong();
        int day = m.captured(1).toInt();
        int month = months.indexOf(m.captured(2)) + 1;
        int year = m.captured(3).toInt();
        return QDate(year,month,day);
    }

    /*rx = dateRegularExpression(Format1);
    m = rx.match(s);
    if (m.hasMatch()) {
        // (02).(08).(2019)
        return QDate::fromString(s,"dd.MM.yyyy");
    }*/

    rx = dateRegularExpression(Format2);
    m = rx.match(s);
    if (m.hasMatch()) {
        // (02).(08).(19)
        int day = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int year = twoDigitYear(m.captured(3).toInt(), minYear);
        return QDate(year,month,day);
    }

    rx = dateRegularExpression(Format3);
    m = rx.match(s);
    if (m.hasMatch()) {
        // 6 авг 19
        QStringList months = ruMonthsShort();
        int day = m.captured(1).toInt();
        int month = months.indexOf(m.captured(2)) + 1;
        int year = twoDigitYear(m.captured(3).toInt(), minYear);
        return QDate(year,month,day);
    }

    rx = dateRegularExpression(Format4);
    m = rx.match(s);
    if (m.hasMatch()) {
        // 9-авг-2019
        QStringList months = ruMonthsShort();
        int day = m.captured(1).toInt();
        int month = months.indexOf(m.captured(2)) + 1;
        int year = m.captured(3).toInt();
        return QDate(year,month,day);
    }

    return QDate();
}

QDateTime DateTime::parseDateTime(const QString &s, int minYear, bool inLocal, bool outLocal)
{

    QRegularExpression rx = dateTimeRegularExpression(FormatRuLocaleLong);
    QRegularExpressionMatch m = rx.match(s);
    if (m.hasMatch()) {
        // 1    2         3         4
        // (22) (августа) (2023) г. (19:35:14)
        QStringList months = ruMonthsLong();
        int day = m.captured(1).toInt();
        int month = months.indexOf(m.captured(2)) + 1;
        int year = m.captured(3).toInt();
        QDate date(year,month,day);
        QTime time = QTime::fromString(m.captured(4),"h:mm:ss");
        QDateTime dateTime(date, time, inLocal ? Qt::LocalTime : Qt::UTC);
        return outLocal ? dateTime.toLocalTime() : dateTime.toUTC();
    }

    rx = dateTimeRegularExpression(FormatRuLocaleShortTwoDigitYear);
    m = rx.match(s);
    if (m.hasMatch()) {
        // 1    2    3    4
        // (05).(08).(19) (3:00)
        int day = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int year = twoDigitYear(m.captured(3).toInt(), minYear);
        QDate date(year,month,day);
        QString time_ = m.captured(4);
        QTime time = QTime::fromString(time_,"h:mm");
        QDateTime dateTime(date, time, inLocal ? Qt::LocalTime : Qt::UTC);
        return outLocal ? dateTime.toLocalTime() : dateTime.toUTC();
    }

    rx = dateTimeRegularExpression(Format5);
    m = rx.match(s);
    if (m.hasMatch()) {
        // 1    2    3    4      5
        // (07).(08).(19) (5:00) (AM)
        int day = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int year = twoDigitYear(m.captured(3).toInt(), minYear);
        QDate date(year,month,day);
        QTime time = parseAmPmTime(m.captured(4), m.captured(5), "h:mm");
        if (!time.isValid()) {
            return QDateTime();
        }
        QDateTime dateTime(date, time, inLocal ? Qt::LocalTime : Qt::UTC);
        return outLocal ? dateTime.toLocalTime() : dateTime.toUTC();
    }

    return QDateTime();
}

QTime DateTime::parseTime(const QString &s)
{
    QRegularExpression rx;
    QRegularExpressionMatch m;
    rx = timeRegularExpression(Format5);
    m = rx.match(s);
    if (m.hasMatch()) {
        //          1      2
        // 07.08.19 (5:00) (AM)
        return parseAmPmTime(m.captured(1),m.captured(2),"h:mm");
    }

    rx = timeRegularExpression(Format6);
    m = rx.match(s);
    if (m.hasMatch()) {
        //          1          2
        // 07.08.19 (09:08:15) (PM)
        return parseAmPmTime(m.captured(1),m.captured(2),"h:mm:ss");
    }
    return QTime();
}

QList<QRegularExpression> DateTime::dateRegularExpressions()
{
    QString dashed_yyyymmdd = "[0-9]{4}-[0-9]{2}-[0-9]{2}";
    QString dotted_ddmmyyyy = "[0-9]{2}[.][0-9]{2}[.][0-9]{4}";
    QList<QRegularExpression> result = {
        // DefaultLocaleLongDate
        // 5 сентября 1974 г.
        dateRegularExpression(FormatRuLocaleLong),
        // DefaultLocaleShortDate
        // 19.11.2058
        QRegularExpression("^" + dotted_ddmmyyyy + "$"),
        // ISODate, ISODateWithMs
        // 2057-11-08
        QRegularExpression("^" + dashed_yyyymmdd + "$"),
        // RFC2822Date
        // 18 Jun 1921
        QRegularExpression("^[0-9]{1,2}\\s(" + enMonthsShort().join("|") + ")\\s[0-9]{4}$"),
        // TextDate
        // Чт май 28 1970
        dateRegularExpression(FormatRuLocaleText),
        // 02.08.19
        dateRegularExpression(Format2),
        // 6 авг 19
        dateRegularExpression(Format3),
        // 9-авг-2019
        dateRegularExpression(Format4)
    };


    return result;
}

QList<QRegularExpression> DateTime::dateTimeRegularExpressions()
{
    QString hm = "[0-9]{1,2}:[0-9]{2}";
    QString hms = "[0-9]{1,2}:[0-9]{2}:[0-9]{2}";
    QString hmsms = hms + "[.][0-9]{3}";
    QString dashed_yyyymmdd = "[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}";
    QString dotted_ddmmyyyy = "[0-9]{1,2}[.][0-9]{1,2}[.][0-9]{4}";

    QString two_digits = "[0-9]{1,2}";
    QString four_digits = "[0-9]{4}";

    QList<QRegularExpression> result = {
        // DefaultLocaleLongDate
        // 5 сентября 1974 г. 1:18:07
        dateTimeRegularExpression(FormatRuLocaleLong),
        // DefaultLocaleShortDate
        // 28.01.2067 21:11
        QRegularExpression("^" + dotted_ddmmyyyy + "\\s" + hm + "$"),
        // ISODate
        // 2057-11-08T17:21:09
        QRegularExpression("^" + dashed_yyyymmdd + "T" + hms + "$"),
        // ISODateWithMs
        // 1978-04-10T04:57:05.185
        QRegularExpression("^" + dashed_yyyymmdd + "T" + hmsms + "$"),
        // RFC2822Date
        // 05 Jan 1998 05:44:32 +0300
        QRegularExpression("^" + two_digits + "\\s(" + enMonthsShort().join("|") + ")\\s" + four_digits + "\\s" + hms + "\\s[+][0-9]{4}$"), // RFC2822Date
        // TextDate
        // Чт май 28 04:55:06 1970
        dateTimeRegularExpression(FormatRuLocaleText),
        // 05.08.19 3:00
        dateTimeRegularExpression(FormatRuLocaleShortTwoDigitYear),
        // 07.08.19 5:00 AM
        dateTimeRegularExpression(Format5),
    };

    return result;
}

QList<QRegularExpression> DateTime::timeRegularExpressions()
{
    QString hm = "[0-9]{1,2}:[0-9]{2}";
    QString hms = hm + ":[0-9]{2}";
    QString hmsms = hms + "[.][0-9]{3}";

    QList<QRegularExpression> result = {
        // 12:24
        QRegularExpression("^(" + hm + ")$"),
        // 20:40:46
        QRegularExpression("^(" + hms + ")$"),
        // 16:10:21.285
        QRegularExpression("^(" + hmsms + ")$"),
        // 12:18 PM
        timeRegularExpression(Format5),
        // 09:08:15 PM
        timeRegularExpression(Format6)
    };
    return result;
}
#endif

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
                    stream << QString("Test%4Sample(Q%4::fromString(\"%1\",%5),\"%2\",Qt::%3),\n")
                              .arg(dateTime.toString(Qt::ISODateWithMs))
                              .arg(dateTime.toString(format))
                              .arg(formatName)
                              .arg("DateTime")
                              .arg("Qt::ISODateWithMs");
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
