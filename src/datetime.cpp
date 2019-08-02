#include "datetime.h"
#include <QDir>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>

namespace {

int twoDigitYear(int year, int minYear) {
    int year_ = (minYear / 100) * 100 + year;
    if (year_ < minYear) {
        year_ = (minYear / 100 + 1) * 100 + year;
    }
    return year_;
}

}

QRegularExpression DateTime::timeZoneRegularExpression()
{
    /*
    https://www.timeanddate.com/time/zones/
    let tzs = [...document.querySelector('#tz-abb tbody').querySelectorAll('tr')].map( tr => tr.querySelector('td').innerText )
    document.body.innerText = JSON.stringify(tzs).replace("[","{").replace("]","}")

    // https://windowsnotes.ru/other/perexodim-na-zimnee-vremya/
    */

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

    return QRegularExpression("\\s(" + tzs.join("|") + "|(" + tzNumeric + ")|(" + tzRtz + "))$");
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
    return {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
}

QStringList DateTime::ruWeekDaysShort()
{
    return {"Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"};
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
        QString time_ = m.captured(4);
        QTime time = QTime::fromString(time_,"h:mm");
        // AM/PM parsing doesn't work on some locales
        QString ap = m.captured(5);
        if (ap == "PM") {
            if (!(time.hour() == 12 && time.minute() == 0)) { // 12:00 PM is noon 12:00 AM is midnight
                time = time.addSecs(12 * 3600);
            }
        } else if (ap == "AM") {
            if (time.hour() == 12 && time.minute() == 0) {
                time = time.addSecs(-12 * 3600);
            }
        } else {
            qDebug() << "AM / PM error";
            return QDateTime();
        }

        QDateTime dateTime(date, time, inLocal ? Qt::LocalTime : Qt::UTC);
        return outLocal ? dateTime.toLocalTime() : dateTime.toUTC();
    }

#if 0
    rx = ruDateTimeRegularExpression(FormatRuLocaleTextDate);
    m = rx.match(s);
    if (m.hasMatch()) {
        // 1    2     3    4          5
        // (Сб) (дек) (23) (19:09:00) (2006)
        QStringList months = ruMonthsShort();
        int day = m.captured(3).toInt();
        int month = months.indexOf(m.captured(2)) + 1;
        int year = m.captured(5).toInt();
        QDate date(year,month,day);
        QTime time = QTime::fromString(m.captured(4),"h:mm:ss");
        QDateTime dateTime(date,time, inLocal ? Qt::LocalTime : Qt::UTC);
        return outLocal ? dateTime.toLocalTime() : dateTime.toUTC();
    }
#endif
    return QDateTime();
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
    QString ms = "[0-9]{1,2}:[0-9]{2}";
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
        QRegularExpression("^" + dotted_ddmmyyyy + "\\s" + ms + "$"),
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

    QDir dir("./data");

    if (!dir.exists()) {
        dir.cdUp();
        dir.mkdir("data");
        dir.cd("data");
    }

    for(int i=0;i<dateFormats.size();i++) {
        Qt::DateFormat format = dateFormats[i];
        QString formatName = dateFormatNames[i];

        QString filePath = dir.filePath(formatName + ".txt");

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "cannot open" << filePath;
            continue;
        }

        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8"));

        for(int j=0;j<1000;j++) {

            int y = 1900 + (rand() % 200);
            int M = 1 + (rand() % 12);
            int d = 1 + (rand() % 28);

            //M = 1 + (i % 12);
            //d = 1 + (i % 28);
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

            bool samplesDate = true;

            stream << QString("Test%4Sample(Q%4::fromString(\"%1\",%5),\"%2\",Qt::%3),\n")
                      .arg(samplesDate ? dateTime.date().toString(Qt::ISODate) : dateTime.toString(Qt::ISODateWithMs))
                      .arg(samplesDate ? dateTime.date().toString(format) : dateTime.toString(format))
                      .arg(formatName)
                      .arg(samplesDate ? "Date" : "DateTime")
                      .arg(samplesDate ? "Qt::ISODate" : "Qt::ISODateWithMs");

        }
    }

    qDebug() << "samples writen";

}
