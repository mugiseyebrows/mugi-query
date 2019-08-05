#include "sqldatatypes.h"

#include <QDateTime>
#include <QLocale>
#include <QDebug>
#include <QTimeZone>
#include <QRegularExpression>
#include "datetime.h"

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

QVariant SqlDataTypes::tryConvert(const QVariant& v, QVariant::Type t,
                                  const QLocale& locale,
                                  bool inLocalTime,
                                  bool outUtc,
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

    // assume two digit year means years in range [currentYear - 50, currentYear + 50)
    // where currentYear is current year rounded to tens
    // TODO: user specified minYear
    static int minYear = ((QDate::currentDate().year() + 5) / 10) * 10 - 50;

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
        QDate date;
        if (DateTime::parseAsDate(s,date,minYear)) {
            if (ok) {
                *ok = true;
            }
            return date;
        }
        if (ok) {
            *ok = false;
        }
        return QVariant();

    } else if (t == QVariant::Time) {

        QTime time;
        if (DateTime::parseAsTime(s,time)) {
            if (ok) {
                *ok = true;
            }
            return time;
        }

        if (ok) {
            *ok = false;
        }
        return QVariant();

    } else if (t == QVariant::DateTime) {

        QDateTime dateTime;
        if (DateTime::parseAsDateTime(s,dateTime,minYear,inLocalTime, outUtc)) {
            if (ok) {
                *ok = true;
            }
            return dateTime;
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
