#include "sqldatatypes.h"

#include <QDateTime>
#include <QLocale>
#include <QDebug>
#include <QTimeZone>

QStringList SqlDataTypes::names()
{
    // ^.*(".*").*$
    // \1,

    QStringList result = {"INT",
                          "DOUBLE",
                          "TEXT",
                          "DATE",
                          "TIME",
                          "DATETIME",
                          "BLOB"};
    return result;
}

QMap<QString,QVariant::Type> SqlDataTypes::mapToVariant()
{
    QMap<QString,QVariant::Type> m;
    m["INT"] = QVariant::Int;
    m["DOUBLE"] = QVariant::Double;
    m["TEXT"] = QVariant::String;
    m["DATE"] = QVariant::Date;
    m["TIME"] = QVariant::Time;
    m["DATETIME"] = QVariant::DateTime;
    m["BLOB"] = QVariant::ByteArray;
    return m;
}


template<typename A, typename B>
QMap<B,A> invert(const QMap<A,B>& vs) {
    QList<A> keys = vs.keys();
    A key;
    QMap<B,A> res;
    foreach(key,keys) {
        res[vs[key]] = key;
    }
    return res;
}

QMap<QVariant::Type, QString> SqlDataTypes::mapFromVariant()
{
    return invert(mapToVariant());
}

QMap<QVariant::Type, QString> SqlDataTypes::mapToDriver(const QString &driver)
{
    QMap<QVariant::Type, QString> m;

    if (driver == "QMYSQL") {
        m[static_cast<QVariant::Type>(QMetaType::UChar)] = "TINYINT UNSIGNED";
        m[static_cast<QVariant::Type>(QMetaType::Char)] = "TINYINT";
        m[static_cast<QVariant::Type>(QMetaType::UShort)] = "SMALLINT UNSIGNED";
        m[static_cast<QVariant::Type>(QMetaType::Short)] = "SMALLINT";
        m[QVariant::UInt] = "INT UNSIGNED";
        m[QVariant::Int] = "INT";
        m[QVariant::ULongLong] = "BIGINT UNSIGNED";
        m[QVariant::LongLong] = "BIGINT";
        m[QVariant::Double] = "DOUBLE";
        m[QVariant::Date] = "DATE";
        m[QVariant::Time] = "TIME";
        m[QVariant::DateTime] = "DATETIME";
        m[QVariant::String] = "TEXT";
        m[QVariant::ByteArray] = "BLOB";
    } else {

        m[QVariant::Int] = "INT";
        m[QVariant::Double] = "DOUBLE";
        m[QVariant::String] = "TEXT";
        m[QVariant::Date] = "DATE";
        m[QVariant::Time] = "TIME";
        m[QVariant::DateTime] = "DATETIME";
        m[QVariant::String] = "TEXT";
        m[QVariant::ByteArray] = "BLOB";

    }


    return m;
}

#include <QRegularExpression>
#include <QDir>
#include <QTextCodec>

void SqlDataTypes::writeDateTimeSamples()
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

        for(int i=0;i<1000;i++) {

            int y = 1900 + (rand() % 200);
            int M = 1 + (rand() % 12);
            int d = 1 + (rand() % 28);

            int h = rand() % 24;
            int m = rand() % 60;
            int s = rand() % 60;
            int ms = rand() % 1000;

            QDate date(y,M,d);
            QTime time(h,m,s,ms);
            QDateTime dateTime(date,time);
            stream << dateTime.toString(format) << "\n";
        }
    }



}

bool testOffset(int id, const QDateTime& d1, const QDateTime& d2, int offset) {

    QDateTime d1_ = d1.toUTC();
    QDateTime d2_ = d2.toUTC();

    int offset_ = d1_.secsTo(d2_);
    if (offset_ != offset) {
        qDebug() << "test" << id << "offset expected" << offset << "actual" << offset_;
    }
    return offset_ == offset;
}

bool testSpec(int id, const QDateTime& d, Qt::TimeSpec spec) {
    Qt::TimeSpec spec_ = d.timeSpec();
    if (spec_ != spec) {
        qDebug() << "test" << id << "spec expected" << spec << "actual" << spec_;
    }
    return spec_ == spec;
}

void SqlDataTypes::tryConvertTest() {

    QLocale locale;

    static QList<Qt::DateFormat> dateFormats = {Qt::TextDate,
                                            Qt::ISODate,
                                            Qt::SystemLocaleShortDate,
                                            Qt::SystemLocaleLongDate,
                                            Qt::DefaultLocaleShortDate,
                                            Qt::DefaultLocaleLongDate,
                                            Qt::RFC2822Date,
                                            Qt::ISODateWithMs};

    bool passed = true;

    bool inLocalDateTime = true;
    bool outLocalDateTime = true;

    QRegularExpression rxTimeWithMs("[0-9]+:[0-9]+:[0-9]+[.][0-9]+");
    QRegularExpression rxTimeWithS("[0-9]+:[0-9]+:[0-9]+");

    QDateTime dlocal(QDate(2000,1,1),QTime(12,0,0),Qt::LocalTime);

    QDateTime dutc(QDate(2000,1,1),QTime(12,0,0),Qt::UTC);

    QString s = dlocal.toString(Qt::ISODate);

    QDateTime d1 = tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();
    QDateTime d2 = tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    QDateTime d3 = tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    QDateTime d4 = tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();

    // d1 TT 12MSK 12MSK
    // d2 FF 12GMT 12GMT
    // d3 FT 12GMT 15MSK
    // d4 TF 12MSK  9GMT

    testSpec(__LINE__,d1,Qt::LocalTime);
    testSpec(__LINE__,d2,Qt::UTC);
    testSpec(__LINE__,d3,Qt::LocalTime);
    testSpec(__LINE__,d4,Qt::UTC);

    testOffset(__LINE__,dlocal,d1,0);
    testOffset(__LINE__,dutc,d2,0);
    testOffset(__LINE__,dutc,d3,0);
    testOffset(__LINE__,dlocal,d4,0);

    int offset = dlocal.timeZone().offsetFromUtc(dlocal);

    testOffset(__LINE__,dlocal,d2,offset);
    testOffset(__LINE__,dlocal,d3,offset);

    qDebug() << d1 << d2 << d3 << d4;

    for(int i=0;i<100;i++) {

        int y = 1900 + (rand() % 200);
        int M = 1 + (rand() % 12);
        int d = 1 + (rand() % 28);

        int h = rand() % 24;
        int m = rand() % 60;
        int s = rand() % 60;
        int ms = rand() % 1000;

        QDate date(y,M,d);
        QTime time(h,m,s,ms);

        /*QDateTime t1(QDate(2000,1,1),QTime(12,0,0),Qt::LocalTime);
        QDateTime t2(QDate(2000,1,1),QTime(12,0,0),Qt::UTC);*/

        QDateTime dateTime(date,time,Qt::UTC);

        // date

        foreach(Qt::DateFormat format, dateFormats) {
            QString s = date.toString(format);

            bool ok;
            QVariant converted = tryConvert(s,QVariant::Date,locale,inLocalDateTime,outLocalDateTime,&ok);

            if (converted.isNull()) {
                passed = false;
                qDebug() << "tryConvert return null for" << date;
                continue;
            }

            QDate date_ = converted.toDate();

            if (!ok) {
                passed = false;
                qDebug() << "tryConvert failed for" << date;
                continue;
            }

            if (date != date_) {
                passed = false;
                qDebug() << "tryConvert converted" << date << "with error" << date_;
                continue;
            }

        }

        static QStringList dateFormats2 = {"yyyy-MM-dd","dd.MM.yyyy"};

        foreach(const QString& format, dateFormats2) {
            QString s = date.toString(format);

            bool ok;
            QVariant converted = tryConvert(s,QVariant::Date,locale,inLocalDateTime,outLocalDateTime,&ok);

            if (converted.isNull()) {
                passed = false;
                qDebug() << "tryConvert return null for" << date;
                continue;
            }

            QDate date_ = converted.toDate();

            if (!ok) {
                passed = false;
                qDebug() << "tryConvert failed for" << date;
                continue;
            }

            if (date != date_) {
                passed = false;
                qDebug() << "tryConvert converted" << date << "with error" << date_;
                continue;
            }

        }

        foreach(Qt::DateFormat format, dateFormats) {

            if (format == Qt::DefaultLocaleLongDate || format == Qt::SystemLocaleLongDate) {
                //TODO: is "7:06:08 MSK" valid time?
                continue;
            }

            QString string = time.toString(format);

            bool hasSeconds = rxTimeWithS.match(string).hasMatch();
            bool hasMilliseconds = rxTimeWithMs.match(string).hasMatch();

            QTime time0 = time;

            //qDebug() << string << hasSeconds << hasMilliseconds;

            if (!hasSeconds && !hasMilliseconds) {
                time0 = QTime(h,m,0);
            } else if (!hasSeconds) {

            } else if (!hasMilliseconds) {
                time0 = QTime(h,m,s);
            }

            bool ok;

            QVariant converted = tryConvert(string, QVariant::Time,locale,inLocalDateTime,outLocalDateTime,&ok);

            if (converted.isNull()) {
                passed = false;
                qDebug() << "tryConvert return null for" << time0 << string;
                continue;
            }

            QTime time_ = converted.toTime();

            if (!ok) {
                passed = false;
                qDebug() << "tryConvert failed for" << time0 << time_ << string << format;
                continue;
            }

            if (time0 != time_) {
                passed = false;
                qDebug() << "tryConvert converted" << time0 << "with error" << time_ << string;
                continue;
            }

        }

        foreach(Qt::DateFormat format, dateFormats) {

            QString string = dateTime.toString(format);

            bool timeWithMs = rxTimeWithMs.match(string).hasMatch();
            bool timeWithS = rxTimeWithS.match(string).hasMatch();

            QDateTime dateTime0 = QDateTime(QDate(y,M,d),
                                            QTime(h,m,timeWithS ? s : 0,timeWithMs ? ms : 0),
                                            Qt::UTC);

            bool ok;
            QVariant converted = tryConvert(string,QVariant::DateTime,locale,inLocalDateTime,outLocalDateTime,&ok);

            if (converted.isNull()) {
                passed = false;
                qDebug() << "tryConvert return null" << dateTime0 << string << format;
                continue;
            }

            QDateTime dateTime_ = converted.toDateTime();

            /*if (dateTime_.timeSpec() == Qt::LocalTime) {
                dateTime_.setTimeZone(QTimeZone(0));
            }*/

            if (!ok) {
                passed = false;
                qDebug() << "tryConvert ok == false" << dateTime0 << string << dateTime_ << format;
                continue;
            }

            if (dateTime0 != dateTime_) {
                passed = false;
                qDebug() << "tryConvert converted with error" << format;
                qDebug() << "in" << dateTime0;
                qDebug() << "in" << string;
                qDebug() << "out" << dateTime_;
                continue;
            }

        }


    } // for

    qDebug() << "SqlDataTypes::tryConvertTest()" << (passed ? "passed" : "failed");
}


QVariant SqlDataTypes::tryConvert(const QVariant& v, QVariant::Type t,
                                  const QLocale& locale,
                                  bool inLocalDateTime,
                                  bool outLocalDateTime,
                                  bool* ok) {

    if (v.isNull()) {
        if (ok) {
            *ok = true;
        }
        return v;
    }

    if (v.type() == t) {
        if (ok) {
            *ok = true;
        }
        return v;
    }

    if (t == QVariant::String) {
        return v.toString();
    }

    if (v.type() != QVariant::String) {
        qDebug() << "t != QVariant::String" << __FILE__ << __LINE__;
        return v;
    }

/*
https://www.timeanddate.com/time/zones/
let tzs = [...document.querySelector('#tz-abb tbody').querySelectorAll('tr')].map( tr => tr.querySelector('td').innerText )
document.body.innerText = '\\\\s(' + tzs.join('|') + '|([+][0-9]{4})' + ')$'
*/

    QRegularExpression tzRx = QRegularExpression("\\s(A|ACDT|ACST|ACT|ACT|ACWST|ADT|ADT|AEDT|AEST|AET|AFT|AKDT|AKST|ALMT|AMST|AMST|AMT|AMT|ANAST|ANAT|AQTT|ART|AST|AST|AT|AWDT|AWST|AZOST|AZOT|AZST|AZT|AoE|B|BNT|BOT|BRST|BRT|BST|BST|BST|BTT|C|CAST|CAT|CCT|CDT|CDT|CEST|CET|CHADT|CHAST|CHOST|CHOT|CHUT|CIDST|CIST|CKT|CLST|CLT|COT|CST|CST|CST|CT|CVT|CXT|ChST|D|DAVT|DDUT|E|EASST|EAST|EAT|ECT|EDT|EEST|EET|EGST|EGT|EST|ET|F|FET|FJST|FJT|FKST|FKT|FNT|G|GALT|GAMT|GET|GFT|GILT|GMT|GST|GST|GYT|H|HDT|HKT|HOVST|HOVT|HST|I|ICT|IDT|IOT|IRDT|IRKST|IRKT|IRST|IST|IST|IST|JST|K|KGT|KOST|KRAST|KRAT|KST|KUYT|L|LHDT|LHST|LINT|M|MAGST|MAGT|MART|MAWT|MDT|MHT|MMT|MSD|MSK|MST|MT|MUT|MVT|MYT|N|NCT|NDT|NFDT|NFT|NOVST|NOVT|NPT|NRT|NST|NUT|NZDT|NZST|O|OMSST|OMST|ORAT|P|PDT|PET|PETST|PETT|PGT|PHOT|PHT|PKT|PMDT|PMST|PONT|PST|PST|PT|PWT|PYST|PYT|PYT|Q|QYZT|R|RET|ROTT|S|SAKT|SAMT|SAST|SBT|SCT|SGT|SRET|SRT|SST|SYOT|T|TAHT|TFT|TJT|TKT|TLT|TMT|TOST|TOT|TRT|TVT|U|ULAST|ULAT|UTC|UYST|UYT|UZT|V|VET|VLAST|VLAT|VOST|VUT|W|WAKT|WARST|WAST|WAT|WEST|WET|WFT|WGST|WGT|WIB|WIT|WITA|WST|WST|WT|X|Y|YAKST|YAKT|YAPT|YEKST|YEKT|Z|([+][0-9]{4}))$");

    static QList<Qt::DateFormat> dateFormats = {Qt::TextDate,
                                            Qt::ISODate,
                                            Qt::SystemLocaleShortDate,
                                            Qt::SystemLocaleLongDate,
                                            Qt::DefaultLocaleShortDate,
                                            Qt::DefaultLocaleLongDate,
                                            Qt::RFC2822Date,
                                            Qt::ISODateWithMs};

    QString s = v.toString();

    if (t == QVariant::Int) {
        return v.toInt(ok);
    } else if (t == QVariant::Double) {

        bool ok_;

        double d = v.toDouble(&ok_);

        if (ok_) {
            if (ok) {
                *ok = true;
            }
            return d;
        }

        d = locale.toDouble(s,&ok_);
        if (ok_) {
            if (ok) {
                *ok = true;
            }
            return d;
        }

        s.replace(",",".");

        return s.toDouble(ok);
    } else if (t == QVariant::Date) {

        foreach(Qt::DateFormat format, dateFormats) {
            QDate d = QDate::fromString(s,format);
            if (d.isValid()) {
                if (ok) {
                    *ok = true;
                }
                return d;
            }
        }

        if (ok) {
            *ok = false;
        }
        return QVariant();

    } else if (t == QVariant::Time) {

        foreach(Qt::DateFormat format, dateFormats) {
            QTime d = QTime::fromString(s,format);
            if (d.isValid()) {
                if (ok) {
                    *ok = true;
                }
                return d;
            }
        }



        if (ok) {
            *ok = false;
        }
        return QVariant();

    } else if (t == QVariant::DateTime) {

        bool hasTimeZone = tzRx.match(s).hasMatch();

        foreach(Qt::DateFormat format, dateFormats) {
            QDateTime d = QDateTime::fromString(s,format);

            QDateTime dout;

            if (!d.isValid()) {
                continue;
            }

            if (hasTimeZone) {
                if (outLocalDateTime) {
                    dout = d;
                } else {
                    dout = d.toUTC();
                }
            } else {
                if (inLocalDateTime && outLocalDateTime) {
                    dout = d;
                } else if (inLocalDateTime && !outLocalDateTime) {
                    dout = d.toUTC();
                } else if (!inLocalDateTime && outLocalDateTime) {
                    dout = QDateTime(d.date(),d.time(),Qt::UTC).toLocalTime();
                } else if (!inLocalDateTime && !outLocalDateTime) {
                    dout = QDateTime(d.date(),d.time(),Qt::UTC);
                }
            }

            if (ok) {
                *ok = true;
            }
            return dout;

        }

        if (ok) {
            *ok = false;
        }

        return QVariant();

    } else if (t == QVariant::ByteArray) {
        if (ok) {
            *ok = true;
        }
        return v.toByteArray();
    }

    return v;
}
