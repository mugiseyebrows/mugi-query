#ifndef DATASTREAMER_H
#define DATASTREAMER_H

#include <QVariant>
#include <QString>

class QSqlQueryModel;
class QTextStream;
class QAbstractItemModel;
class QSqlDatabase;

#include "dataformat.h"
#include "formats.h"

class DataStreamer
{
public:


    static void stream(const QSqlDatabase& db, QTextStream& stream, QSqlQueryModel* model, DataFormat::Format format, const QString& table,
                 QList<bool> data, QList<bool> keys, DataFormat::ActionType action, const QLocale &locale, QString &error);

    static QStringList variantListToStringList(const QVariantList &values,
                                               DataFormat::Format format,
                                               const Formats &formats,
                                               const QLocale &locale,
                                               QString& error);
    static QString variantToString(const QVariant &value, DataFormat::Format format,
                                   const Formats& formats, const QLocale& locale,
                                   QString& error);


    static QString createTableStatement(const QSqlDatabase &db, const QString& table, const QStringList &columns, const QStringList &types, bool ifNotExists);

    static QString stream(DataFormat::Format format, const QSqlDatabase &db, QAbstractItemModel *model, int rowCount, const QString &table, const QStringList &columns, const QStringList &types, int dataColumns, const QLocale &locale, bool *hasMore, QString &error);
};


#endif // DATASTREAMER_H
