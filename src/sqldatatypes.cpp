#include "sqldatatypes.h"

#include <QDateTime>
#include <QLocale>
#include <QDebug>
#include <QTimeZone>
#include <QRegularExpression>
#include "datetime.h"
#include "drivernames.h"

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

QMap<QString, QMetaType::Type> SqlDataTypes::mapToVariant()
{
    QMap<QString,QMetaType::Type> m;
    m["INT"] = QMetaType::Int;
    m["DOUBLE"] = QMetaType::Double;
    m["TEXT"] = QMetaType::QString;
    m["DATE"] = QMetaType::QDate;
    m["TIME"] = QMetaType::QTime;
    m["DATETIME"] = QMetaType::QDateTime;
    m["BLOB"] = QMetaType::QByteArray;
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

QMap<QMetaType::Type, QString> SqlDataTypes::mapFromVariant()
{
    return invert(mapToVariant());
}

QMap<QMetaType::Type, QString> SqlDataTypes::mapToDriver(const QString &driver)
{
    QMap<QMetaType::Type, QString> m;

    if (driver == DRIVER_MYSQL) {
        m[QMetaType::UChar] = "TINYINT UNSIGNED";
        m[QMetaType::Char] = "TINYINT";
        m[QMetaType::UShort] = "SMALLINT UNSIGNED";
        m[QMetaType::Short] = "SMALLINT";
        m[QMetaType::UInt] = "INT UNSIGNED";
        m[QMetaType::Int] = "INT";
        m[QMetaType::ULongLong] = "BIGINT UNSIGNED";
        m[QMetaType::LongLong] = "BIGINT";
        m[QMetaType::Double] = "DOUBLE";
        m[QMetaType::QDate] = "DATE";
        m[QMetaType::QTime] = "TIME";
        m[QMetaType::QDateTime] = "DATETIME";
        m[QMetaType::QString] = "TEXT";
        m[QMetaType::QByteArray] = "BLOB";
    } else {

        m[QMetaType::Int] = "INT";
        m[QMetaType::Double] = "DOUBLE";
        m[QMetaType::QString] = "TEXT";
        m[QMetaType::QDate] = "DATE";
        m[QMetaType::QTime] = "TIME";
        m[QMetaType::QDateTime] = "DATETIME";
        m[QMetaType::QString] = "TEXT";
        m[QMetaType::QByteArray] = "BLOB";

    }


    return m;
}

QVariant SqlDataTypes::tryConvert(const QVariant& v, QMetaType::Type t,
                                  const QLocale& locale,
                                  int minYear,
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

    if (t == QMetaType::QString) {
        return v.toString();
    }

    if (v.metaType().id() != QMetaType::QString) {
        qDebug() << "t != QMetaType::QString" << __FILE__ << __LINE__;
        return v;
    }

    QString s = v.toString();

    if (t == QMetaType::Int) {
        return v.toInt(ok);
    } else if (t == QMetaType::Double) {

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
    } else if (t == QMetaType::QDate) {
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

    } else if (t == QMetaType::QTime) {

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

    } else if (t == QMetaType::QDateTime) {

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

    } else if (t == QMetaType::QByteArray) {
        if (ok) {
            *ok = true;
        }
        return v.toByteArray();
    }

    return v;
}
