#include "sqldatatypes.h"

#include <QDateTime>
#include <QLocale>
#include <QDebug>

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


QVariant SqlDataTypes::tryConvert(const QVariant& v, QVariant::Type t, const QLocale& locale, bool* ok) {

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

        QString s = v.toString();

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
        QDate d1 = QDate::fromString(v.toString(),Qt::ISODate);
        QDate d2 = QDate::fromString(v.toString(),Qt::SystemLocaleDate);
        if (ok) {
            *ok = d1.isValid() || d2.isValid();
        }
        return d1.isValid() ? d1 : d2;
    } else if (t == QVariant::Time) {
        if (ok) {
            *ok = true;
        }
        return v.toTime();
    } else if (t == QVariant::DateTime) {
        if (ok)
            *ok = true;
        return v.toDateTime();
    } else if (t == QVariant::ByteArray) {
        if (ok)
            *ok = true;
        return v.toByteArray();
    }

    return v;
}
