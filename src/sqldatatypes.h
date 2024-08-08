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

    static QMap<QString,QMetaType::Type> mapToVariant();

    static QMap<QMetaType::Type, QString> mapFromVariant();

    static QMap<QMetaType::Type, QString> mapToDriver(const QString& driver);

    static QVariant tryConvert(const QVariant &v, QMetaType::Type t, const QLocale &locale, int minYear,
                               bool inLocalTime, bool outUtc, bool *ok);


};

#endif // BASICTYPES_H
