#ifndef BASICTYPES_H
#define BASICTYPES_H

#include <QStringList>
#include <QVariant>
#include <QMap>

class QLocale;

class SqlDataTypes
{
public:

    static QStringList names();

    static QMap<QString,QVariant::Type> mapToVariant();

    static QMap<QVariant::Type, QString> mapFromVariant();

    static QMap<QVariant::Type, QString> mapToDriver(const QString& driver);

    static QVariant tryConvert(const QVariant &v, QVariant::Type t, const QLocale &locale,
                               bool inLocalDateTime, bool outLocalDateTime, bool *ok = 0);

    static void tryConvertTestMain();

    static void writeDateTimeSamples();

    static void tryConvertTestDateTimeLocalUtc();
};

#endif // BASICTYPES_H
